#include <CommandParser.h>
#include <Servo.h>
#include <Button.h>
#include <PID_v1.h>

typedef CommandParser<> MyCommandParser;

MyCommandParser parser;
Servo starter;
Servo throttle;

#define OUTPUT_VOLTAGE_DIVIDER 11.0
#define REF_VOLTAGE 5.0
float read_output_voltage()
{
  return (float)analogRead(7) * REF_VOLTAGE * OUTPUT_VOLTAGE_DIVIDER / 1023.0;
}

Button start_button(2);


void cmd_throttle(MyCommandParser::Argument *args, char *response) {
  throttle.write((uint32_t)args[0].asUInt64);
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_starter(MyCommandParser::Argument *args, char *response) {
  starter.write((uint32_t)args[0].asUInt64);
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

#define STARTER_ON 90
#define STARTER_OFF 10

#define THROTTLE_MIN 60
#define THROTTLE_MAX 120
#define THROTTLE_IDLE 90


#define ENGINE_RUNNING_VOLTAGE 10
#define STARTER_RUNNING_VOLTAGE 1

double target_voltage = 12.0;
double pid_p = 0.1;
double pid_d = 0.5;
double pid_i = 0; //4;
double output_voltage = 0;
double output_throttle = 0;

//Specify the links and initial tuning parameters
PID throttlePID(&output_voltage, &output_throttle, &target_voltage, pid_p, pid_i, pid_d, DIRECT);


void setup() {
  Serial.begin(9600);
  while (!Serial);

  starter.attach(5);
  starter.write(STARTER_OFF);
  throttle.attach(4);
  throttle.write(THROTTLE_IDLE);
  start_button.begin();


  output_voltage = read_output_voltage();
  throttlePID.SetMode(AUTOMATIC);

  parser.registerCommand("throttle", "u", &cmd_throttle);
  parser.registerCommand("starter", "u", &cmd_starter);
}

void loop() {
  if (Serial.available()) {
    char line[128];
    size_t lineLength = Serial.readBytesUntil('\n', line, 127);
    line[lineLength] = '\0';

    char response[MyCommandParser::MAX_RESPONSE_SIZE];
    parser.processCommand(line, response);
    Serial.println(response);
  }

if(start_button.pressed())
  starter.write(STARTER_ON);
if(start_button.released())
  starter.write(STARTER_OFF);

  output_voltage = read_output_voltage();
  if(output_voltage > ENGINE_RUNNING_VOLTAGE)
  {
    throttlePID.Compute();
    throttle.write(constrain(THROTTLE_IDLE + output_throttle, THROTTLE_MIN, THROTTLE_MAX));
  }
  else
  {
    throttle.write(THROTTLE_IDLE);
  }
  Serial.print(output_voltage);
  Serial.print('\t');
  Serial.println(output_throttle);
  delay(10);

}
