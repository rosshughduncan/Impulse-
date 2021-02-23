#include "aservelibs/aservelib.h"
#include <stdio.h>

// === NOTES ===================================================

// Copyright © Ross Duncan 2016
// Any comments marked in *** asterisks *** correspond to procedure boxes in the flowchart

// === FUNCTION DECLARATIONS =========================================
int GetTempo();
void CalculateFrequencies();
float ChangeAmplitude(int amplitude1, int amplitude2, int nowEditing);
int ChangeWaveType(int waveType1, int waveType2, int nowEditing);
void PlayNotes(int tempo, int step);
int GetControl(int controlNumber);

// === GLOBAL VARIABLES ==============================================
// *** Create array storing step properties ***
// A 4-dimensional array is used to hold this data:
// There are 5 sequences.There are 5 rows for each sequence, containing the layers. Each layer has 8 steps. Each step has 3 rows: frequency, amplitude and wave type.
float steps[5][5][8][3];
// *** Create variables storing current sequence and layer ***
// The user initially starts working on Sequence 1 at Layer 1, at Step 1 (1 is represented by 0 as the array is zero-indexed)
int sequence = 0;
int layer = 0;

// ===== FUNCTION DEFINITIONS ========================================
int main ()
{
    int sequence1 = 0; // Used for detecting if the user wants to change sequence/layer
    int sequence2; // sequence1 stores the encoder value from the previous iteration of the loop. sequence2 is the current setting
    int layer1 = 0;
    int layer2 = 0;
    int tempo;
    int nowEditing = -1; // Stores which step are editing if a step is selected with a button
    int buttonInput; // Stores the input of the button
    int amplitude1 = GetControl(103); // Stores values of rotary 3 before it's changed
    int amplitude2; // Stores values of rotary 3 after it's changed
    int waveType1 = GetControl(104); // Stores values of rotary 4 before it's changed
    int waveType2; // and after
    int step = 0; // Stores current step
    int rotary5 = GetControl(25); // Stores initial setting of rotary 5
    
    // We initialise all of the amplitudes to 0 and all of the wave types to sine
    for (int count1 = 0; count1 <= 4; count1++)
    {
        for (int count2 = 0; count2 <= 4; count2++)
        {
            for (int count3 = 0; count3 <= 7; count3++)
            {
                steps[count1][count2][count3][1] = 0.1; // All amplitudes to 0.1
                steps[count1][count2][count3][2] = 0; // All wave types to sine (0)
            }
        }
    }
    
    
    
    // *** Give user instructions ***
    printf("\nWelcome to Impulse++. You have 5 sequences available to use. Each sequence has 5 layers and each layer has 8 steps. To change sequence, move rotary encoder 1. To change the layer, move rotary encoder 2.\n\n");
    printf("Use the first 8 sliders to change the frequency of each step. Button 9 locks/unlocks the sliders so you can change the frequencies - ensure that button 9 is pressed before you move a slider. The sliders can handle up to 127 notches - every 4 notches is a semitone.  Middle C is 25. Press the button under a slider to change that step's amplitude and wave type [sine (0)/square (1)/saw (2)] using rotary encoders 3 and 4 respecitvely - press the button again to exit this edit mode. Please note that all of the amplitudes are initialised to 0.1 and the wave types are sine by default. [PRESS ENTER TO CONTINUE]\n");
    // Allow the user to press enter
    char enter;
    scanf("%c", &enter);
    printf("Use the modulation wheel to adjust the tempo - move it to the maximum position to exit the program.\n\nUse rotary encoder 5 to transpose the layer you're editing in semitones - you may only transpose up to start with but then down once you've increased the control. Be aware that sliders set to 0 will also be transposed, as 0 means 0 Hz.\n\nBEFORE YOU BEGIN: SET ALL CONTROLS TO 0. Remember to engage button 9 to change any frequencies and to deselect any slider buttons when not altering a step's amplitude or wave type to avoid confusion. THIS MAY BECOME LOUD; turn your volume down! Press ENTER when you're ready to begin.");
    scanf("%c", &enter);
    
    // Infinite loop
    while (true)
    {
        // *** Take input from modulation wheel ***
        tempo = GetTempo();
        if (tempo == 1)
        {
            // If the returned value is 1 (meaning the modulation wheel is at 0), the program exits
            break;
        }
        
        // *** Take input from rotary 1 ***
        sequence2 = GetControl(21);
        // If they have changed increased the encoder, increment the sequence number
        if (sequence2 > sequence1)
        {
            // If the sequence number is already 4, don't change it
            if (sequence < 4)
            {
                sequence++;
            }
        }
        // Otherwise, if they have decreased the encoder, decrement the sequence number
        else if (sequence2 < sequence1)
        {
            // If the sequence number is already 0, don't change it
            if (sequence > 0)
            {
                sequence--;
            }
        }
        // Set the previous encoder value to the current value
        sequence1 = sequence2;
        
        // *** Take input from rotary 2 ***
        // Do the same check for the layers as for the sequences
        layer2 = GetControl(22);
        if (layer2 > layer1)
        {
            if (layer < 4)
            {
                layer++;
            }
        }
        else if (layer2 < layer1)
        {
            if (layer > 0)
            {
                layer--;
            }
        }
        layer1 = layer2;
        
        // *** Take input from sliders ***
        CalculateFrequencies();
        
        // *** Take input from slider buttons ***
        for (int counter = 0; counter <= 7; counter++)
        {
            buttonInput = GetControl(counter + 51);
            // If the button is pressed, we register that the corresponding step is to be edited
            if (buttonInput > 0)
            {
                nowEditing = counter;
            }
        }
        
        // If we are editing a step, take input from rotary 3 and 4
        if (nowEditing != -1)
        {
            amplitude2 = GetControl(23);
            waveType2 = GetControl(24);
            // Work out what the user is changing the amplitude and wave type values to and insert the new values into the array
            float newAmplitude = ChangeAmplitude(amplitude1, amplitude2, nowEditing);
            int newWaveType = ChangeWaveType(waveType1, waveType2, nowEditing);
            amplitude1 = newAmplitude;
            waveType1 = newWaveType;
        }
        
        // *** Take input from rotary 5 ***
        int rotary5Reading = GetControl(25);
        int difference;
        // If the rotary has been moved, shift the notes by a number of semitones
        if (rotary5Reading != rotary5)
        {
            // Work out difference of the value of rotary 5 before and after it was changed
            difference = rotary5Reading - rotary5;
            int frequency;
            int setting;
            // Loop through all steps in the layer
            for (int counter = 0; counter <= 7; counter++)
            {
                // Calculate the new value
                frequency = steps[sequence][layer][counter][0];
                setting = frequency / 21;
                setting = setting + (difference * 4);
                frequency = setting * 21;
                // If frequency is set too low or too high, throw error
                if (frequency < 0)
                {
                    printf("\nERROR: Transposition too low");
                    break;
                }
                else if (frequency > 20000)
                {
                    printf("\nERROR: Transposition too high");
                    break;
                }
                // If there's no error, store the new value in the array
                else
                {
                    steps[sequence][layer][counter][0] = frequency;
                }
            }
            rotary5 = rotary5Reading;
        }
        
        // Play all notes currently under the step currently selected, from each of the 5 layers
        PlayNotes(tempo, step);
        // Increment the step number
        step++;
        // If we've reached the end of the sequence, set the step number so we're back at the beginning for the next iteration
        if (step == 8)
        {
            step = 0;
            if (nowEditing != -1)
            {
                // If we are editing a step (if a slider button is pressed), display the step number as well
                printf("\nNow editing Sequence %d, Layer %d, STEP %d", sequence + 1, layer + 1, nowEditing + 1);
            }
            else
            {
                // Otherwise, just show the sequence and layer
                printf("\nNow editing Sequence %d, Layer %d", sequence + 1, layer + 1);
            }
        }
        // Reset the nowEditing ID to null in case it's not pressed next time
        nowEditing = -1;
    }
    printf("\n");
    return 0;
}

int GetTempo()
{
    // Take reading from modulation wheel, multiply it by 5 and subtract it from 1000 to get the tempo
    int reading = GetControl(1);
    int tempo = 700 - reading * 5.5;
    return tempo;
}

void CalculateFrequencies()
{
    // Do not calculate any frequencies unless button 9 is pressed
    int button9Reading = GetControl(59);
    if (button9Reading == 127)
    {
        // Variable for taking the reading of the slider
        int reading;
        // Varible for storing the result of the calculation
        int calculation;
        // We take the input of every slider and multiply it by 140 to get the frequency
        for (int stepNo = 0; stepNo <= 7; stepNo++)
        {
            // As the control number of the lowest slider is 41, we adjust the counter to reference it
            reading = GetControl(stepNo + 41);
            calculation = reading * 21;
            // We store the frequency in the array under the frequency row of the step
            steps[sequence][layer][stepNo][0] = calculation;
        }
    }
}

float ChangeAmplitude(int amplitude1, int amplitude2, int nowEditing)
{
    // Work out the difference between the value before the control was changed and the value after
    float amplitudeChange = amplitude2 - amplitude1;
    float oldAmplitude = steps[sequence][layer][nowEditing][1];
    // and calculate a new amplitude. Set the value to 127 if it's too large and 0 if it's too small
    float newAmplitude = oldAmplitude + amplitudeChange;
    if (newAmplitude < 0)
    {
        newAmplitude = 0;
    }
    else if (newAmplitude > 127)
    {
        newAmplitude = 127;
    }
    // Store new amplitude in the array. Ensure the value is scaled as the actual amplitude must be between 0 and 1
    steps[sequence][layer][nowEditing][1] = newAmplitude / 127;
    return newAmplitude;
}

int ChangeWaveType(int waveType1, int waveType2, int nowEditing)
{
    // Work out the difference between the value before the control was changed and the value after
    int waveTypeChange = waveType2 - waveType1;
    int oldWaveType = steps[sequence][layer][nowEditing][2];
    // Calculate the new wave type. Set the value to 2 if it's too large and 0 if it's to small
    int newWaveType = oldWaveType + waveTypeChange;
    if (newWaveType < 0)
    {
        newWaveType = 0;
    }
    else if (newWaveType > 2)
    {
        newWaveType = 2;
    }
    steps[sequence][layer][nowEditing][2] = newWaveType;
    return newWaveType;
}

void PlayNotes(int tempo, int step)
{
    int frequency;
    float amplitude;
    int waveType;
    // Work out parameters for the oscillator and play each oscillator
    for (int counter = 0; counter <= 4; counter++)
    {
        frequency = steps[sequence][counter][step][0];
        amplitude = steps[sequence][counter][step][1];
        waveType = steps[sequence][counter][step][2];
        aserveOscillator(counter, frequency, amplitude, waveType);
    }
    // Wait for some time, according to the tempo
    aserveSleep(tempo);
}

// Scans the control 20 times to get the correct value
// in order to get around a bug within Aserve that means that it only registers the correct control value after a number of checks
int GetControl(int controlNumber)
{
    int result;
    for (int counter = 0; counter < 20; counter++)
    {
        result = aserveGetControl(controlNumber);
    }
    return result;
}