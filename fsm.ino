/* sinbrive July 2018
 * Simple FSM for arduino
 * forked from minsuklee/protocol_fsm 
 * Simulate 4 states expecting 4 events
 * Serial for tests (the exmaple was to make led flashing based on number of clik on a button)
 */

#define TEST_FSM

#define NUM_STATE   4
#define NUM_EVENT   4

// States
enum proto_state 
  { ZERO_BLINK, ONE_BLINK, 
    TWO_BLINK, THREE_BLINK 
  };     

// Events
enum proto_event { ONE_CLICK, TWO_CLICK, THREE_CLICK, TIMEOUT };

char *st_name[] =  { "zero Blink", "one Blink", "two Blink", "three Blink" };

char *ev_name[] =  { "one Push", "two Push ", "three Push", "TIMEOUT"};

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
    Serial.println(st_name[ZERO_BLINK]);
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
  //  ONE_CLICK,                    TWO_CLICK,                 THREE_CLICK,                TIMEOUT

  // - state 0 
  {{ entryOneBlink, ONE_BLINK }, { entryTwoBlink, TWO_BLINK },      { entryThreeBlink, THREE_BLINK },      { NULL, ZERO_BLINK }},

  // - one click
  {{ NULL, ONE_BLINK }, { entryTwoBlink, TWO_BLINK },      { entryThreeBlink, THREE_BLINK }, { entryZeroBlink, ZERO_BLINK }},

  // - two clicks
  {{ entryOneBlink, ONE_BLINK }, { NULL, TWO_BLINK },      { entryThreeBlink, THREE_BLINK },      { entryZeroBlink, ZERO_BLINK }},
  
    // - three clicks
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
        event.event=proto_event((number-1)%3);
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
