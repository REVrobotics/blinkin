void setupMode()
{
  inSetup = !inSetup;

  if (inSetup)
  {
    // Update status LED color
    setStatusSetup();

    // Set the current display pattern to the test pattern (a pattern which shows the two team colors)
    testPatternDisplay = TESTPATTERN;

    // Save the current settings incase the user doens't want to save on exit
    //COLOR1temp = COLOR1;
    //COLOR2temp = COLOR2;
    //NUM_LEDStemp = NUM_LEDS;
    
  }
  // Exit set-up
  else
  {
    // User exited setup



    // Restore old settings if new ones weren't saved
    //COLOR1 = COLOR1temp;
    //COLOR2 = COLOR2temp;
    //NUM_LEDS = NUM_LEDStemp;

    //addressableStrip = EEPROM.read(SS_EE);
    COLOR1 = EEPROM.read(COLOR1_EE);
    COLOR2 = EEPROM.read(COLOR2_EE);
    //NUM_LEDS = EEPROM.read(LED_EE);
    //noSignalPatternDisplay = EEPROM.read(PATTERN_EE); 

    SetupCustomPalette(colorList[COLOR1], colorList[COLOR2]);
    
    // Update Status LED Color
    setStatusRun();  
    
    //resume normal operation
  }
}

void readUserInputs()
{
  //read Pot value and translate to colors/strip Length

//  lengthHistory.unshift(map(analogRead(LENGTH_PIN), 0, 1024, 1, 240));
//
//    //check that the pattern value has been stable 
//    for (int i = 0 ; i< lengthHistory.capacity() ; i++){
//      if (lengthHistory[0] != lengthHistory[i])
//        lengthStable = false;
//    }
//    
//    if (lengthStable){
//      if (NUM_LEDS > lengthHistory[0]){
//         //need loop to only update pixels to black that are > then new length
//         Black();
//         FastLED.show();
//      }
//      //NUM_LEDS = lengthHistory[0];
//      //FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);           
//    }     
//
//    lengthStable = true;


    color1History.unshift(map(analogRead(COLOR1_PIN), 0, 1024, 0, (ARRAY_SIZE(colorList))));
    
    //check that the pattern value has been stable 
    for (int i = 0 ; i< color1History.capacity() ; i++){
      if (color1History[0] != color1History[i])
        lengthStable = false;
    }
    
    if (color1Stable){
      COLOR1 = color1History[0];         
    }   

    color1Stable = true;

    color2History.unshift(map(analogRead(COLOR2_PIN), 0, 1024, 0, (ARRAY_SIZE(colorList))));

    //check that the pattern value has been stable 
    for (int i = 0 ; i< color2History.capacity() ; i++){
      if (color2History[0] != color2History[i])
        lengthStable = false;
    }
    
    if (color2Stable){
      COLOR2 = color2History[0];         
    }   

    SetupCustomPalette(colorList[COLOR1], colorList[COLOR2]);
    color2Stable = true;  
}

void buttonHandler()
{

  // If both buttons are pressed increment the both buttons pressed counter
  if((digitalRead(MODE_PIN) == LOW) && (digitalRead(SS_PIN) == LOW))
  {
    programButtonHoldCount++;
  }

//  // Strip Select Button Logic
//  if(digitalRead(SS_PIN) == LOW)
//  {
//    ssButtonHoldCount++;
//  }
//  else //SS_PIN == HIGH
//  {
//    if ((ssButtonHoldCount > 5) && (programButtonHoldCount == 0) )
//    {
//      if (inSetup == true)
//      {
//        //increment output pattern
//        testPatternDisplay = constrain(testPatternDisplay++, 0, 99);
//      }
//      else
//      {
//        toggleStripSelect();
//      }
//      ssButtonHoldCount = 0;
//    }
//  }

  // Strip Select Button Logic
  if(digitalRead(SS_PIN) == LOW)
  {
    ssButtonHoldCount++;
    
    if ((digitalRead(MODE_PIN) == HIGH) && (programButtonHoldCount == 0) && (inSetup == false))
    {
      if (ssButtonHoldCount > 80)
      {
        ssButtonHoldCount = 0;
        toggleStripSelect();
        stripTransistion = true;
      }
    }
  }
  else // SS_PIN == HIGH
  {
    if ((ssButtonHoldCount > 20) && (programButtonHoldCount == 0) )
    {
      if ((noSignal == true) && (inSetup == true))
      {
        if (stripTransistion == true)
        {
          stripTransistion = false;
        }
        else
        {
        //increment output pattern
        testPatternDisplay = constrain(noSignalPatternDisplay++, 0, 99);
        }
      }
      ssButtonHoldCount = 0;
    }
  }


  // Mode Button Logic
  if(digitalRead(MODE_PIN) == LOW)
  {
    modeButtonHoldCount++;
    
    if ((digitalRead(SS_PIN) == HIGH) && (programButtonHoldCount == 0))
    {
      if (modeButtonHoldCount > 180)
      {
        modeButtonHoldCount = 0;
        setupMode();
        setupTransistion = true;
      }
    }
  }
  else // MODE_PIN == HIGH
  {
    if ((modeButtonHoldCount > 20) && (programButtonHoldCount == 0) )
    {
      if ((noSignal == true) && (inSetup == true))
      {
        if (setupTransistion == true)
        {
          setupTransistion = false;
        }
        else
        {
        //increment output pattern
        testPatternDisplay = constrain(noSignalPatternDisplay--, 0, 99);
        }
      }
      modeButtonHoldCount = 0;
    }
  }

  // Both Buttons Pressed Logic
  if ((programButtonHoldCount > 150) && (inSetup == true))
  {
    setStatusEEPROM(); 

    if (programButtonHoldCount > 180)
    {
      //write EEPROM
      saveDefaults();
      
      setupMode();
    }
    
    ssButtonHoldCount = 0;
    modeButtonHoldCount = 0;
  }

  // Reset all the counters if neither button is pressed
  if(digitalRead(MODE_PIN) == HIGH && digitalRead(SS_PIN) == HIGH)
  {
    programButtonHoldCount = 0;
    ssButtonHoldCount = 0;
    modeButtonHoldCount = 0;
  }

}



//void SetupCustomPalette(uint8_t HUE_color1, uint8_t HUE_color2)
void SetupCustomPalette(CRGB color1, CRGB color2)
{
  teamPalette = CRGBPalette16(
                                 color2,  color1,  color1,  color1, 
                                 color1, color1, color1,  color1,
                                 color1,  color2,  color2,  color2,
                                 color2, color1, color1,  color2 );
}

void testPattern()
{  
  if (addressableStrip == true) {

    uint8_t colorIndex = 1;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( teamPalette, colorIndex, BRIGHTNESS, currentBlending);
        colorIndex += 3; 
        //fill_rainbow( leds, NUM_LEDS, gHue, 7);
    }
  }
  else {
    // Long blink Primary and short Blink Secondary, then crossfade, repeat?
    gHue = gHue + 1;
    // Use FastLED automatic HSV->RGB conversion
    showAnalogRGB( CHSV( gHue, 255, 255) );
    delay(5);
  }
}

void saveDefaults()
{
  if (testPatternDisplay != TESTPATTERN)
    noSignalPatternDisplay = testPatternDisplay;

  COLOR1temp = COLOR1;
  COLOR2temp = COLOR2; 
  
  //SetupCustomPalette(colorList[COLOR1], colorList[COLOR2]);
  
  //EEPROM write takes 3.3ms
  if(writeEEPROM)
  {
      EEPROM.update(SS_EE, addressableStrip);
      EEPROM.update(COLOR1_EE, COLOR1);
      EEPROM.update(COLOR2_EE, COLOR2);
      EEPROM.update(LED_EE, NUM_LEDS);
      EEPROM.update(PATTERN_EE, noSignalPatternDisplay);
  }
  
}

void initEEPROM()
{
  if((digitalRead(MODE_PIN) == LOW) && (digitalRead(SS_PIN) == LOW))
  {
    //EEPROM write takes 3.3ms
    if(writeEEPROM)
    {      
      EEPROM.write(SS_EE, addressableStrip);
      EEPROM.write(COLOR1_EE, COLOR1);
      EEPROM.write(COLOR2_EE, COLOR2);
      EEPROM.write(LED_EE, NUM_LEDS);
      EEPROM.write(PATTERN_EE, noSignalPatternDisplay);
    }
  }
  else
  {
    // Check if defaults have been stored in EEPROM
    if (EEPROM.read(COLOR1_EE) == 0xFF) //defaults have not been saved, load variables into them
    {
      //EEPROM write takes 3.3ms
      if(writeEEPROM)
      {      
        EEPROM.write(SS_EE, addressableStrip);
        EEPROM.write(COLOR1_EE, COLOR1);
        EEPROM.write(COLOR2_EE, COLOR2);
        EEPROM.write(LED_EE, NUM_LEDS);
        EEPROM.write(PATTERN_EE, noSignalPatternDisplay);
        //EEPROM.write(5, 0x42);
      }
    }
    else  //reload from saved values
    {
      addressableStrip = EEPROM.read(SS_EE);
      COLOR1 = EEPROM.read(COLOR1_EE);
      COLOR2 = EEPROM.read(COLOR2_EE);
      //NUM_LEDS = 240;//EEPROM.read(LED_EE);
      noSignalPatternDisplay = EEPROM.read(PATTERN_EE);
    }
  }
}

void setStatusCommand() {
  // "Purple"
  digitalWrite(sREDPIN, LOW);
  digitalWrite(sGREENPIN, HIGH);
  digitalWrite(sBLUEPIN, LOW);
}

void setStatusSetup() {
  // "Yellow"
  digitalWrite(sREDPIN, LOW);
  digitalWrite(sGREENPIN, LOW);
  digitalWrite(sBLUEPIN, HIGH);
}

void setStatusRun() {
  // "Blue"
  digitalWrite(sREDPIN, HIGH);
  digitalWrite(sGREENPIN, HIGH);
  digitalWrite(sBLUEPIN, LOW);
}

void setStatusMode() {

  if (inSetup == false) {
    setStatusRun();
  }   // "Blue"
  else {
    setStatusSetup();
  }  // "Yellow"
}


void setStatusNoSignal() {
  // "Blink Output"

  if (inSetup == false)
  {
    digitalWrite(sGREENPIN, HIGH);
    digitalWrite(sREDPIN, HIGH);

    if (digitalRead(sBLUEPIN) == HIGH) {
      digitalWrite(sBLUEPIN, LOW);
    }
    else {
      digitalWrite(sBLUEPIN, HIGH);
    }
  }
  else
  {
    digitalWrite(sBLUEPIN, HIGH);

    if (digitalRead(sGREENPIN) == HIGH) {
      digitalWrite(sGREENPIN, LOW);
      digitalWrite(sREDPIN, LOW);
    }
    else {
      digitalWrite(sGREENPIN, HIGH);
      digitalWrite(sREDPIN, HIGH);
    }
  }
}

void setStatusError() {
  // "Red"
  digitalWrite(sREDPIN, LOW);
  digitalWrite(sGREENPIN, HIGH);
  digitalWrite(sBLUEPIN, HIGH);
}


void setStatusEEPROM() {
  // "cyan"
  digitalWrite(sREDPIN, HIGH);
  digitalWrite(sGREENPIN, LOW);
  digitalWrite(sBLUEPIN, LOW);
}
