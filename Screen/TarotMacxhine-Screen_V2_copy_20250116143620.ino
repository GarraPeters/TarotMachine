#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#define USE_HSPI_FOR_EPD
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEW075T7 800x480, EK79655 (GD7965)
SPIClass hspi(HSPI);

String deckFull[80][4] = { 
{"Start","0","",""},
{"The Fool","8","Spontaneity, Adventure, Innocence, Beginnings, Playfulness, Optimism","Naive, Overly Optimistic, Foolish, Careless"},
{"The Magician","20","Willpower, Manifestation, Skill, Self Confidence","Trickery, Lies, Manipulation, Wasted Talent, Insecurity"},
{"The High Priestess","28","Intuition, Mystery, Spirituality, Wisdom, Inner Voice","Repressed Intuition, Low Self-Esteem, Superficiality, Confusion"},
{"The Empress","40","Divine Feminine, Fertility, Abundance, Nurturing, Motherly","Greedy, Smothering, Insecurity, Neglect, Ignoring the Bigger Picture"},
{"The Emperor","48","Stability, Authority, Control, Order, Practicality","Rigid, Stubborn, Narrow-Minded, Control Freak, Recklessness"},
{"The Hierophant","60","Tradition, Knowledge, Beliefs, Conformity, Good Counsel","Rebellion, Non-Conformity, Unorthodoxy, Ignorance, Counseling Needed"},
{"The Lovers","68","Love, Partnerships, Relationships, Romance, Equal Partnership","Disharmony, Imbalance, Incompatible Partner, Bad Choices"},
{"The Chariot","80","Success, Willpower, Ambition, Drive, Movement","Forceful, No Direction, Obstacles, Delays, Procrastination"},
{"Strength","88","Courage, Confidence, Inner Strength, Compassion, Control","Self-Doubt, Weakness, Depression, Cowardice, Fear of Standing On Your Own"},
{"The Hermit","100","Self-Reflection, Introspection, Solitude, Meditation, Contemplation","Loneliness, Isolation, Withdrawn, Unwanted Loneliness, Need to Re-evaluate"},
{"Wheel of Fortune","108","Change, Fate, Luck, Fortune, Prosperity, Cycles","Bad Luck, Lack of Control, Stuck in a Rut, Refusing Change"},
{"Justice","120","Justice, Accountability, Law, Fair Treatment, Karma","Injustice, Dishonesty, Unfair Treatment, Corruption, Avoiding Accountability"},
{"The Hanged Man","128","Sacrifice, Waiting, Perspective, Suspension, Calm Before the Storm","Stalling, Disinterest, Delays, Hang Ups, Impatience"},
{"Death","140","Transformation, Endings, Change, Release, Major Changes","Fear of Change, Stagnation, Living in the Past, Need to Let Go"},
{"Temperance","148","Balance, Peace, Patience, Moderation, Tolerance, Understanding","Imbalance, Excess, Recklessness, Need for Moderation, Impatience"},
{"The Devil","160","Obsession, Addiction, Dependency, Materialism, Lust","Freedom, Facing Fears, Overcoming Obsession, Independence"},
{"The Tower","168","Disaster, Upheaval, Sudden Change, Chaos, Destruction","Averting Disaster, Resisting Change, Time to Grow and Move On"},
{"The Star","180","Hope, Inspiration, Renewal, Focusing on Dreams & Goals","Despair, Negativity, Losing Hope, Despondent"},
{"The Moon","188","Illusion, Intuition, Mystery, Secrets, Something Hidden","Fear, Deception, Misunderstanding, Insecurity, Clarity"},
{"The Sun","200","Happiness, Success, Vitality, New Beginnings, Family, Joy","Blocked Happiness, Pessimism, Unrealistic Expectations, Family Trouble"},
{"Judgment","208","Self-Evaluation, Renewal, Reflection, Good Karma, Fair Decisions","Self-Doubt, Failure to Learn Lessons, Bad Karma, Poor Judgement"},
{"The World","220","Completion, Achievement, Wholeness, Success, Cycles","Lack of Closure, Incomplete, Need to Accept Changes, Emptiness"},

{"Wild Card","228","",""},

{"Ace of Wands","240","Inspiration, Creative Spark, New Initiative, Enthusiasm, Energy, Communication","Delays, Lack of Passion, Creative Blocks, Hesitancy, Feeling Uninspired, No Communication"},
{"Two of Wands","248","Momentum, Confidence, Expansion, Foresight, Counseling or Help from a Third Party","Limitations, Lack of Progress, Obstacles, Frustrations, Third-Party Interference"},
{"Three of Wands","260","Momentum, Confidence, Expansion, Foresight, Counseling or Help from a Third Party","Limitations, Lack of Progress, Obstacles, Frustrations, Third-Party Interference"},
{"Four of Wands","270","Community, Celebrations, Stability, Marriage, Solid Foundations","Lack of Support, Instability, Transience, Non-Marital Commitment, Living Arrangements"},
{"Five of Wands","278","Conflict, Competition, Tension, Aggression, Solutions Through Disputes","End of Conflict, Cooperation, Agreement, Avoidance of Conflict, Unresolved Disputes"},
{"Six of Wands","288","Success, Victory, Recognition, Acclaim, Overcoming Stress","Failure, Lack of Recognition, Rewards Delayed, Disillusionment"},
{"Seven of Wands","300","Protectiveness, Defending Oneself, Communicative Mastery, Under Attack","Surrender, Defeat, Lack of Self-Belief, Feeling Drained & Stressed"},
{"Eight of Wands","308","Speed, Progress, Quick Decisions, Excitement, Expansion Through Communication or Travel","Waiting, Slow Progress, Chaos, Delays, Frustrations in Communication, Travel Delays"},
{"Nine of Wands","318","Last Stand, Persistence, Resilience, Preparation, Battles Behind You","Stubbornness, Defensiveness, Refusal to Compromise, Paranoia from Past Battles"},
{"Ten of Wands","328","Burden, Responsibility, Struggle, Duty, Loving Responsibilities, Promotions, Rewards for Hard Work","Failure to Delegate, Overburdened, Collapse, Stress, Rewards Delayed"},
{"Page of Wands","340","Adventure, Excitement, Fresh Ideas, Cheerfulness, Travel, News, Communication","Impatient, Lacking Ideas, Laziness, Distraction, Delays or Problems in Communication"},
{"Knight of Wands","348","Courageous, Energetic, Charming, Rebellious, Full of Ideas and Good at Communicating","Arrogant, Reckless, Impatient, Jumping from Idea to Idea, Need for Follow Through"},
{"Queen of Wands","360","Confident, Passionate, Determined, Social, Talkative, Creative","Demanding, Vengeful, Low Confidence, Jealous, Gossipy, Burns Out Quickly"},
{"King of Wands","368","Leadership, Vision, Control, Boldness, Talkative, Creative, Reliable","Forceful, Dominating, Tyrant, Unreliable, Smooth Talker Who is Insincere"},

{"Ace of Cups","380","New love, spiritual awakening, intuition, opportunity, emotional openness","Emotional reserve, hesitancy, introspection, drained feelings, re-evaluation"},
{"Two of Cups","388","Mutual attraction, partnership, harmony, unity, connection","Misunderstanding, individual growth, self-love, recalibration, personal space"},
{"Three of Cups","400","Celebration, community, collaboration, joy, unity","Reflection, solo journey, finding oneself, personal boundaries, self-awareness"},
{"Four of Cups","408","Introspection, meditation, re-evaluation, contemplation, soul-searching","New perspectives, awakening, action, realization, outer-world focus"},
{"Five of Cups","420","Transition, change, personal growth, emotional evolution, life lessons","Healing, release, positive outlook, acceptance, new connections"},
{"Six of Cups","428","Memories, past connections, innocence, joy, nostalgia","Growth, present focus, lessons learned, future outlook, personal evolution"},
{"Seven of Cups","438","Possibilities, dreams, choices, imagination, exploration","Focus, decision-making, clarity, purpose, direction"},
{"Eight of Cups","448","Evolution, search for purpose, spiritual journey, transition, growth","Re-evaluation, second chances, introspection, new discoveries, inner reflection"},
{"Nine of Cups","458","Contentment, gratitude, emotional fulfillment, wishes, satisfaction","Inner search, soul growth, refining desires, self-love, patience"},
{"Ten of Cups","468","Family joy, bliss, harmony, unity, fulfillment","Personal path, individual happiness, self-love, discovery, new beginnings"},
{"Page of Cups","480","Intuition, creativity, emotional exploration, curiosity, dreams","Emotional immaturity, introspection, daydreaming, missed opportunities, distraction"},
{"Knight of Cups","488","Romantic, dreamer, idealistic, kindness, offers","Disconnected, mood swings, unrealistic, indecisive, scattered emotions"},
{"Queen of Cups","498","Compassion, intuition, emotional balance, nurture, inner feelings","Emotional insecurity, introspection, moodiness, withdrawal, clouded intuition"},
{"King of Cups","508","Diplomacy, emotional maturity, calm, balance, wisdom","Emotional manipulation, moody, withdrawn, disengagement, imbalance"},

{"Ace of Pentacles","518","Opportunity, potential, prosperity, growth, new beginnings","Re-evaluation, patience, planning, groundwork, re-routing"},
{"Two of Pentacles","528","Flexibility, balance, multitasking, adaptability, juggling","Prioritizing, simplifying, focus, grounding, streamlining"},
{"Three of Pentacles","540","Collaboration, teamwork, skill, expertise, building","Refinement, niche skills, personal projects, individual contribution, recalibration"},
{"Four of Pentacles","548","Stability, security, saving, foundation, protection","Sharing, generosity, open-mindedness, trust, investments"},
{"Five of Pentacles","560","Challenge, perseverance, resilience, growth, lessons","Recovery, support, community, breakthrough, optimism"},
{"Six of Pentacles","568","Giving, prosperity, sharing, balance, kindness","Self-care, boundaries, introspection, personal wealth, re-balance"},
{"Seven of Pentacles","580","Patience, reward, growth, investment, nurturing","Re-strategizing, change of plans, new approaches, re-evaluation, innovation"},
{"Eight of Pentacles","592","Mastery, dedication, craftsmanship, development, hard work","Broadening horizons, seeking passion, diversification, exploration, new interests"},
{"Nine of Pentacles","600","Independence, luxury, self-reliance, achievement, contentment","Exploration, communal joy, shared luxury, collective achievements, social connections"},
{"Ten of Pentacles","610","Abundance, family, legacy, security, comfort","Exploration, new ventures, diversifying, self-investment, flexibility"},
{"Page of Pentacles","620","Opportunity, manifestation, skills, dedication, new beginnings","Daydreaming, missed opportunities, impracticality, lack of commitment, procrastination"},
{"Knight of Pentacles","632","Reliability, responsibility, dedication, routine, efficiency","Stagnation, laziness, fear of change, feeling stuck, resistance"},
{"Queen of Pentacles","640","Nurturing, practicality, motherly, abundance, groundedness","Imbalance, dependency, neglect, smothering, scarcity mindset"},
{"King of Pentacles","650","Mastery, abundance, security, leadership, entrepreneur","Stubbornness, dependency on material, lethargy, stagnant, inflexible"},

{"Ace of Swords","660","Insight, clarity, breakthrough, truth, fresh ideas","Delayed understanding, new approaches, inner clarity, contemplation, reflection"},
{"Two of Swords","670","Decision, balance, truce, duality, peace","Exploration, open-mindedness, insights, personal truth, discovery"},
{"Three of Swords","680","Transformation, growth, understanding, healing, self-discovery","Recovery, release, emotional freedom, optimism, resilience"},
{"Four of Swords","690","Rest, rejuvenation, reflection, meditation, peace","Activity, social connections, energy renewal, exploration, engagement"},
{"Five of Swords","698","Strategy, perspective, lessons, experience, choices","Reconciliation, understanding, release, compromise, harmony"},
{"Six of Swords","710","Transition, progress, change, healing journey, discovery","Reflection, internal journey, personal truths, inner peace, self-discovery"},
{"Seven of Swords","720","Strategy, caution, discretion, tactics, wisdom","Clarity, honesty, open dialogue, trust, transparency"},
{"Eight of Swords","728","Reflection, introspection, temporary pause, personal boundaries, re-evaluation","Empowerment, self-belief, freedom, breakthrough, confidence"},
{"Nine of Swords","740","Introspection, mindfulness, self-exploration, concerns, soul-searching","Healing, relief, hope, release, rejuvenation"},
{"Ten of Swords","750","Closure, endings, rebirth, transformation, renewal","Revival, new dawn, optimism, resilience, fresh perspectives"},
{"Page of Swords","760","Curiosity, mental exploration, new ideas, communication, learning","Gossip, disconnected thoughts, distractions, lack of direction, impulsive words"},
{"Knight of Swords","768","Action, decisiveness, ambition, fast thinking, assertiveness","Impulsiveness, unpreparedness, hasty decisions, aggressive actions, conflict"},
{"Queen of Swords","780","Clarity, independence, sharp mind, communication, truth","Coldness, miscommunication, guardedness, isolation, bias"},
{"King of Swords","788","Intellect, clear thinking, authority, truth, decisiveness","Manipulation, over-analyzing, detachment, harsh judgment, unyielding"}
};

// Network credentials
const char* ssid     = "<WIFI NETWORK NAME>";
const char* password = "<WIFI NETWORK PASSWORD>";

// OpenAI API key
const char* apiKey = "<OPENAI_API_KEY>";

// telegram bot URL
const String reportUrl = "<TELEGRAM_BOT_URL>";

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>

int values[6];

HardwareSerial espSerial(2);

void setup()
{

  hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
  display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));


  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  display.setRotation(0);
  display.setTextWrap(true);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeSerifBoldItalic12pt7b);

  display.setFullWindow();

  display.setPartialWindow(0, 0, display.width(), display.height());

 // Initialize Serial
  Serial.begin(19200);
  espSerial.begin(9600,SERIAL_8N1,16,17);

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop()
{
   
  if(espSerial.available()) {
    // String received = espSerial.readStringUntil('\n');
    String received = espSerial.readString();
    received.trim();
 
    if (received != "" && received != "RESET" ) {
      // displayText(received);
      extractValues(received, values, 6);
      displayText("Working...");
      getFromChatGPT(values[0],values[1],values[2],values[3],values[4],values[5]);
    } else {
      if (WiFi.status() != WL_CONNECTED) {
        displayText("OFFLINE");
      } else {
        displayText("    ");
      }
    }

  }

}

void getFromChatGPT(int card1, bool card1inverted, int card2, bool card2inverted, int card3, bool card3inverted )
{
   // Send request to OpenAI API
  String inverted1 = card1inverted ? " (reversed)":" ";
  String inverted2 = card2inverted ? " (reversed)":" ";
  String inverted3 = card3inverted ? " (reversed)":" ";


  String inputText = "in no more than 600 characters, without using emoji, interpret this tarot reading:  " + deckFull[card1][0] + inverted1 + ", " + deckFull[card2][0] + inverted2 + ", " + deckFull[card3][0] + inverted3;
  String apiUrl = "https://api.openai.com/v1/chat/completions";

  String payload = "{\"model\": \"gpt-4o-mini\",\"messages\": [{\"role\": \"system\",\"content\": \"You are a flirty talking raven who reads tarot cards\"},{\"role\": \"user\",\"content\": \"" + inputText + "\"}]}";

  HTTPClient http;
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(apiKey));
  
  // for sending to telegram bot.
  
  HTTPClient http2;
  http2.begin(reportUrl);
  http2.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);
  String response = http.getString();
  if (httpResponseCode == 200) {
    // String response = http.getString();
  
    // Parse JSON response
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    String outputText = jsonDoc["choices"][0]["message"]["content"];
    // Serial.println(outputText);
    displayText("    ");
    delay(1000);

    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setPartialWindow(0, 0, display.width(), display.height());
    displayText(outputText);

    // Send to telegram bot
    String cleanedOutput = outputText;
    cleanedOutput.replace("\""," ");
    cleanedOutput.replace("\n", "");
    cleanedOutput.replace("\r", "");
    String reportPayload = "{\"chat_id\": \"-615948739\", \"text\": \"" + cleanedOutput + "\", \"disable_notification\": false}";
    http2.POST(reportPayload);
    
  } else {
    // Serial.printf("Error %i \n", httpResponseCode);
    // displayText(response);
    displayText("ERROR");
  }
}

void displayText(String inputString)
{
  int16_t tbx, tby; uint16_t tbw, tbh;
  String formatedInputString = splitStringToLines(inputString, 70, 5);
  display.getTextBounds(inputString, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(formatedInputString);
  }
  while (display.nextPage());
}

void displaySpread(int card1, int card1inverted, int card2, int card2inverted, int card3, int card3inverted )
{
  String line1 = deckFull[card1][0]+": "+deckFull[card1][2];
  String formattedText1 = splitStringToLines(line1, 70, 5);

  String line2 = deckFull[card2][0]+": "+deckFull[card2][2];
  String formattedText2 = splitStringToLines(line2, 70, 5);

  String line3 = deckFull[card3][0]+": "+deckFull[card3][2];
  String formattedText3 = splitStringToLines(line3, 70, 5);

  String fullRead = "    Situation:" + formattedText1 + "    Obsticle:" + formattedText2 + "    Advice:" + formattedText3;

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(fullRead, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.firstPage();
  do
  {
    // display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.println("    Situation:");
    display.print(formattedText1);
    display.println("    Obsticle:");
    display.print(formattedText2);
    display.println("    Advice:");
    display.print(formattedText3);
  }
  while (display.nextPage());
}

int extractValues(String input, int* outputArray, int maxValues) {
  int count = 0; // Number of values extracted
  int lastIndex = 0;
  
  // Iterate through the string to find commas
  while (lastIndex < input.length() && count < maxValues) {
    int commaIndex = input.indexOf(',', lastIndex);
    
    // If no more commas are found, process the last value
    if (commaIndex == -1) {
      commaIndex = input.length();
    }
    
    // Extract the substring and convert to integer
    String valueStr = input.substring(lastIndex, commaIndex);
    outputArray[count++] = valueStr.toInt();
    
    // Move the lastIndex forward
    lastIndex = commaIndex + 1;
  }
  
  return count; // Return the number of extracted values
}

String splitStringToLines(const String &input, int maxLineLength, int spacesAtStart) {
  String result = "";                   // Final result string
  // String spaces = String(" ", spacesAtStart);  // Create spaces to prepend
  String spaces = "";  // Create spaces to prepend
  int start = 0;                        // Start index for the current line
  
  for (int i = 0; i <= spacesAtStart; i++) {
      spaces += " "; 
    }

  while (start < input.length()) {
    int end = start + maxLineLength;    // Tentative end index for the current line
    if (end >= input.length()) {
      result += spaces + input.substring(start) + "\n"; // Add remaining text
      break;
    }
    
    int lastSpace = input.lastIndexOf(' ', end); // Find the last space before end
    if (lastSpace < start) { // If no space is found, break the word (edge case)
      lastSpace = end;
    }
    
    result += spaces + input.substring(start, lastSpace) + "\n"; // Add line to result
    // result += "    " + input.substring(start, lastSpace) + "\n"; // Add line to result
    start = lastSpace + 1; // Move to the next part of the string
  }
  
  return result;
}

