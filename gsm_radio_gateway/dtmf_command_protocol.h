#ifndef DTMF_COMMAND_PROTOCOL_H
#define DTMF_COMMAND_PROTOCOL_H

enum
{
  WAIT_START_SEQ = 0,
  RECEIVING_START_SEQ,
  RECEIVING_COMMAND_CODE,
  RECEIVING_COMMAND_ARG,
  COMMAND_END
};

#define START_SEQ_BUF_LEN 10
#define START_SEQ_LEN 3
#define START_SEQ "***"

#define COMMAND_CODE_BUF_LEN 10
#define CODE_ARG_DELIMITER '#'

#define COMMAND_ARG_BUF_LEN 20
#define COMMAND_END_SYMBOL '#'

#define COMMAND_NUM 10

class DTMF_command_protocol
{
  private:
    int state = WAIT_START_SEQ;

    char* receiving_symbol_ptr = start_seq_buf;

    char start_seq_buf[START_SEQ_BUF_LEN];
    char* start_seq_ptr = start_seq_buf;

    char command_code_buf[COMMAND_CODE_BUF_LEN];

    char command_arg_buf[COMMAND_ARG_BUF_LEN];

    uint32_t command_start_time = 0;
    uint32_t command_max_dur = 15000;
    uint32_t pause_time = 5000;
    bool pause_handled = false;
  public:

    char* command_keys[COMMAND_NUM] = {};
    void (*command_handlers[COMMAND_NUM])(char*) = {};
    void (*start_seq_handler)() = 0;
    void (*command_end_handler)() = 0;
    void (*pause_handler)() = 0;

    void reset()
    {
      Serial.println("reseting");
      receiving_symbol_ptr = start_seq_buf;
      state = WAIT_START_SEQ;
    }

    void update(char c)
    {
      Serial.print("state: ");
      Serial.println(state);
      command_start_time = millis();
      pause_handled = false;
      switch (state)
      {
        case WAIT_START_SEQ:
          if (receiving_symbol_ptr - start_seq_buf >= START_SEQ_BUF_LEN - 2)
            reset();

          *receiving_symbol_ptr = c;
          *(receiving_symbol_ptr + 1) = 0;
          receiving_symbol_ptr++;
          start_seq_ptr = constrain(receiving_symbol_ptr - START_SEQ_LEN, start_seq_buf, start_seq_buf + START_SEQ_BUF_LEN - 2);

          Serial.write(start_seq_buf, strlen(start_seq_buf));
          Serial.println();

          if (!strncmp(start_seq_ptr, START_SEQ, START_SEQ_LEN))
          {
            state = RECEIVING_START_SEQ;
            if (start_seq_handler)
              start_seq_handler();
          }
          break;

        case RECEIVING_START_SEQ:
          if (receiving_symbol_ptr - start_seq_buf >= START_SEQ_BUF_LEN - 2)
            reset();

          *receiving_symbol_ptr = c;
          *(receiving_symbol_ptr + 1) = 0;
          receiving_symbol_ptr++;
          start_seq_ptr = constrain(receiving_symbol_ptr - START_SEQ_LEN, start_seq_buf, start_seq_buf + START_SEQ_BUF_LEN - 2);

          if (strncmp(start_seq_ptr, START_SEQ, START_SEQ_LEN))
          {
            state = RECEIVING_COMMAND_CODE;
            receiving_symbol_ptr = command_code_buf;
            *receiving_symbol_ptr = c;
            *(receiving_symbol_ptr + 1) = 0;
            receiving_symbol_ptr++;
          }
          break;

        case RECEIVING_COMMAND_CODE:
          if (c == CODE_ARG_DELIMITER)
          {
            state = RECEIVING_COMMAND_ARG;
            receiving_symbol_ptr = command_arg_buf;
            break;
          }
          if (receiving_symbol_ptr - command_code_buf >= COMMAND_CODE_BUF_LEN - 2)
            reset();

          *receiving_symbol_ptr = c;
          *(receiving_symbol_ptr + 1) = 0;
          receiving_symbol_ptr++;
          break;

        case RECEIVING_COMMAND_ARG:
          if (c == CODE_ARG_DELIMITER)
          {
            state = COMMAND_END;
            break;
          }
          if (receiving_symbol_ptr - command_arg_buf >= COMMAND_ARG_BUF_LEN - 2)
            reset();

          *receiving_symbol_ptr = c;
          *(receiving_symbol_ptr + 1) = 0;
          receiving_symbol_ptr++;
          break;

        default: break;
      }
    }

    void handle()
    {
      if (receiving_symbol_ptr != start_seq_buf && millis() - command_start_time > command_max_dur)
      {
        if (command_end_handler)
            command_end_handler();
        reset();
      }
      if (state != WAIT_START_SEQ && millis() - command_start_time > pause_time && !pause_handled)
      {
        if (pause_handler)
          pause_handler();
        pause_handled = true;
      }
      switch (state)
      {
        case RECEIVING_START_SEQ:
          break;
        case COMMAND_END:
          for (int i = 0; i < COMMAND_NUM; i++)
          {
            if (command_keys[i] && !strcmp(command_code_buf, command_keys[i]) && command_handlers[i])
              command_handlers[i](command_arg_buf);
          }
          //if (command_end_handler)
          //  command_end_handler();
          reset();
          break;
        default: break;
      }
    }
};

#endif
