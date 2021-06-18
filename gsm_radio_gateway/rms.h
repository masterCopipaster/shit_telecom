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


#endif _RMS_H
