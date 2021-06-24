#ifndef _RMS_H
#define _RMS_H

template <typename ntype> class rmean
{
  private:
  ntype sum = 0;
  ntype exp = 0;
  ntype alpha = 1;

  public:

  ntype update(ntype n)
  {
    sum = sum * alpha + n; 
    exp = exp * alpha + 1;
    return getval();
  }

  ntype getval()
  {
    return sum/exp;
  }

  void set_alpha(ntype a)
  {
    alpha = a;
  }
};

template <typename ntype, int n> class rmean_casc
{
  private:
  rmean<ntype> cascades[n];

  public:

  ntype update(ntype v)
  {
    cascades[0].update(v);
    for(int i = 1; i < n; i++)
    {
      cascades[i].update(cascades[i - 1].getval());
    }
    return cascades[n - 1].getval();
  }

  ntype getval()
  {
    return cascades[n-1].getval();
  }

  void set_alpha(ntype a)
  {
    for(int i = 0; i < n; i++)
    {
      cascades[i].set_alpha(a);
    }
  }
  void set_casc_alpha(ntype a, int i)
  {
     cascades[i].set_alpha(a);
  }
};

template <typename ntype, int n = 1> class rms
{
  private:
  rmean_casc<ntype, n> mean;
  
  public:
  ntype update(ntype v)
  {
    return sqrt(mean.update(v*v));
  }

  ntype getval()
  {
    return sqrt(mean.getval());
  }

  void set_alpha(ntype a)
  {
    mean.set_alpha(a);
  }

  void set_casc_alpha(ntype a, int i)
  {
     mean.set_casc_alpha(a, i);
  }
};

template <typename ntype, int n = 1> class rms_dc_removal
{
  private:
  rmean_casc<ntype, n> mean;
  rmean_casc<ntype, n> dc_mean;
  ntype constr;
  
  public:
  ntype update(ntype v)
  {
    v -= dc_mean.update(v);
    v = constrain(v, -constr, constr);
    return sqrt(mean.update(v*v));
  }

  ntype getval()
  {
    return sqrt(mean.getval());
  }

  void set_constr(ntype c)
  {
     constr = c;
  }
  
  void set_alpha(ntype a)
  {
    mean.set_alpha(a);
    dc_mean.set_alpha(a);
  }

  void set_casc_alpha(ntype a, int i)
  {
    mean.set_casc_alpha(a, i);
    dc_mean.set_casc_alpha(a, i);
  }
};

template <int ncasc = 1> class rms_switch
{
  private:
  bool on = false;
  bool switch_on = false;
  bool switch_off = false;
  float snr_on = 1.7;
  float snr_off = 1.05;
  float snr_diff_on = 1;
  int active_undersample = 20;
  int undersample_counter = active_undersample - 1;
  rms_dc_removal<float, ncasc> rms;
  rms_dc_removal<float, ncasc> rms_noise;
  
  public:

  void set_main_rms_alpha(float a)
  {
    rms.set_alpha(a);
  }
  void set_main_rms_casc_alpha(float a, int i)
  {
    rms.set_casc_alpha(a, i);
  }
  void set_noise_rms_alpha(float a)
  {
    rms_noise.set_alpha(a);
  }
  void set_noise_rms_casc_alpha(float a, int i)
  {
    rms_noise.set_casc_alpha(a, i);
  }
  void update(float v)
  {

    switch_on = false;
    switch_off = false;
    
    rms.update(v);
    if(!on)
      rms_noise.update(v);
    else
    {
      if(!undersample_counter)
        rms_noise.update(v);
      undersample_counter = undersample_counter ? undersample_counter - 1 : active_undersample - 1;
    }
    
    if(rms.getval() / snr_on > rms_noise.getval() && rms.getval() - rms_noise.getval() > snr_diff_on  && !on)
    {
      on = true;
      switch_on = true;
    }
    
    if(rms.getval() / snr_off < rms_noise.getval() && on)
    {
      on = false;
      switch_off = true;
    }
  }

  void set_snr_on(float v)
  {
    snr_on = v;
  }
  void set_snr_off(float v)
  {
    snr_off = v;
  }
  void set_snr_diff_on(float v)
  {
    snr_diff_on = v;
  }
  void set_active_undersample(int n)
  {
    active_undersample = n;
  }

  void set_main_rms_constr(float c)
  {
    rms.set_constr(c);
  }
  void set_noise_rms_constr(float c)
  {
    rms_noise.set_constr(c);
  }

  bool ison()
  {
    return on;
  }
  bool is_switched_on()
  {
    return switch_on;
  }
  bool is_switched_off()
  {
    return switch_off;
  }

  void print_vals()
  {
    Serial.print(rms.getval());
    Serial.print(" ");
    Serial.println(rms_noise.getval());
  }
  
};

#endif _RMS_H
