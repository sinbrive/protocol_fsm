#define TEST_FSM

#define NUM_STATE   4
#define NUM_EVENT   4

// States
enum proto_state 
  { ZERO_BLINK, ONE_BLINK, 
    TWO_BLINK, THREE_BLINK 
  };     

// Events
enum proto_event { onePUSH, twoPUSH, threePUSH, TIMEOUT };

char *st_name[] =  { "zeroBlink", "oneBlink", "twoBlink", "threeBlink" };

char *ev_name[] =  { "onePush", "twoPush", "threePush", "TIMEOUT"};

struct state_action {           // Protocol FSM Structure
    void (* action)(void *p);
    enum proto_state next_state;
};

struct p_event {                // Event Structure
    enum proto_event event;
    void *data;
    int size;
};

unsigned int stateTimeout=3000;

enum proto_state c_state = ZERO_BLINK;     // States

unsigned long referenceTime=millis();

struct p_event *eventp;

static void entryZeroBlink(void *p)
{
    stateTimeout = 5000;
    referenceTime=millis();
    char const buffer = sprintf("Current Stat = %s\n", st_name[c_state]);
    Serial.println(buffer);
    //Serial.println(st_name[ZERO_BLINK]);
}


static void entryOneBlink(void *p)
{
    stateTimeout = 5000;
    referenceTime=millis();
    Serial.println(st_name[ONE_BLINK]);    
}

static void entryTwoBlink(void *p)
{
    stateTimeout = 5000;
    referenceTime=millis();
     Serial.println(st_name[TWO_BLINK]);
}

static void entryThreeBlink(void *p)
{
    stateTimeout = 5000;
    referenceTime=millis();
     Serial.println(st_name[THREE_BLINK]);
}



struct state_action p_FSM[NUM_STATE][NUM_EVENT] = {
  //  for each event:
  //  onePUSH,                    twoPUSH,                 threePUSH,                TIMEOUT

  // - zero push
  {{ entryOneBlink, ONE_BLINK }, { entryTwoBlink, TWO_BLINK },      { entryThreeBlink, THREE_BLINK },      { NULL, ZERO_BLINK }},

  // - one push
  {{ NULL, ONE_BLINK }, { entryTwoBlink, TWO_BLINK },      { entryThreeBlink, THREE_BLINK }, { entryZeroBlink, ZERO_BLINK }},

  // - two push
  {{ entryOneBlink, ONE_BLINK }, { NULL, TWO_BLINK },      { entryThreeBlink, THREE_BLINK },      { entryZeroBlink, ZERO_BLINK }},
  
    // - three push
  {{ entryOneBlink, ONE_BLINK }, { entryTwoBlink, TWO_BLINK },      { NULL, THREE_BLINK },      { entryZeroBlink, ZERO_BLINK }},
};

struct p_event *get_event(void)
{
    static struct p_event event;    // not thread-safe
 
    #ifdef TEST_FSM
      
      event.data = NULL;
      event.size = 0;
    
      if (Serial.available() > 0) 
      {
        int number = Serial.parseInt(); 
        event.event=proto_event((number-1)%2);
      }
      else 
      {
        if ((millis()-referenceTime)>stateTimeout)
        {
           event.event=TIMEOUT;
        }
        else return NULL;
      }
    #endif
    return &event;
}

void setup()
{
  #ifdef TEST_FSM  
    Serial.begin(9600);
    Serial.println(st_name[ZERO_BLINK]);
  #endif
  
}

void loop()
{
       
     /* Step 0: Get Input Event */
    if((eventp = get_event()) != NULL) {
      
//        #ifdef TEST_FSM  
//          char const buffer = printf("Current Stat = %s\n", st_name[c_state]);
//          //Serial.println(st_name[c_state]);
//        #endif    
        
        /* Step 1: Do Action */
        if (p_FSM[c_state][eventp->event].action) {
            p_FSM[c_state][eventp->event].action(eventp);
        }
        else
        {
          #ifdef TEST_FSM  
//             Serial.print("No Action for this event\n");
          #endif
        }             
         /* Step 2: Set Next State */
       c_state = p_FSM[c_state][eventp->event].next_state;
        
    }
}
