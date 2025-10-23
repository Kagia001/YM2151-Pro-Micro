
	/**
	*	YM2151 - Chip Controller Software for ARDUINO
	* (C) 2025  Karl Ragnar Giese
	*	(C) 2016  Marcel Weiß
	*
	*	This program is free software : you can redistribute it and / or modify
	*	it under the terms of the GNU General Public License as published by
	*	the Free Software Foundation, either version 3 of the License, or
	*	(at your option) any later version.
	*
	*	This program is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	*	GNU General Public License for more details.
	*
	*	You should have received a copy of the GNU General Public License
	*	along with this program.If not, see <http://www.gnu.org/licenses/>.
	*/


#include <midi_Settings.h>
#include <midi_Namespace.h>
#include <midi_Message.h>
#include <midi_Defs.h>


#include  <MIDI.h>
#include  <USB-MIDI.h>
#include  <Arduino.h>
#include  <avr/pgmspace.h>
#include  <Wire.h>

#include "YM2151Driver.h" 
#include "NotePool.h"
#include "SysExHandler.h"
#include "PCHandler.h"
#include "CCHandler.h"
#include "EPROMManager.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIDIN);  // Serial1 for 32u4
USBMIDI_CREATE_DEFAULT_INSTANCE();
	

	void setup() {

		
		YM2151.begin();
		YM2151.initLFO();
		MIDI.begin(MIDI_CHANNEL_OMNI); 
		MIDIDIN.begin(MIDI_CHANNEL_OMNI);
		

		//init
		EPROMManager.init();
		YM2151Driver.init();
		NotePool.init();
		CCHandler.init();
		PCHandler.init();
		SysExHandler.init();

		//set Handlers
		MIDI.setHandleSystemExclusive(handle_sysex);
		MIDI.setHandleNoteOn(handle_NoteOn);
		MIDI.setHandleNoteOff(handle_NoteOff);
		MIDI.setHandleProgramChange(handle_PC);
		MIDI.setHandleControlChange(handle_CC);

		MIDIDIN.setHandleSystemExclusive(handle_sysex);
		MIDIDIN.setHandleNoteOn(handle_NoteOn);
		MIDIDIN.setHandleNoteOff(handle_NoteOff);
		MIDIDIN.setHandleProgramChange(handle_PC);
		MIDIDIN.setHandleControlChange(handle_CC);
	} 

	void loop() {
		MIDI.read();
		MIDIDIN.read();
	}

  void loopLissasjous() {
		YM2151DriverClass d = YM2151Driver;

		NotePool.setMode(true); // Not Poly!
		d.setAlgorithm(0, 0);
		d.setAlgorithm(0, 1);
		d.setOpActive(0, 0, 1<<6);
		d.setOpActive(0, 1, 1<<6);
		d.setOpActive(0, 2, 1<<6);
		d.setOpActive(0, 3, 1<<6);
		d.setOpActive(1, 0, 1<<6);
		d.setOpActive(1, 1, 1<<6);
		d.setOpActive(1, 2, 1<<6);
		d.setOpActive(1, 3, 1<<6);

		d.setTone(0, 69, 0);
		d.setTone(1, 69, 2);
		d.setPan(0,1*32);
		d.setPan(1,3*32);
		d.noteOn(0);
		d.noteOn(1);

		while (1) {
			MIDI.read();
			MIDIDIN.read();
		}
	}

	void loopNervigerTon() {
		YM2151DriverClass d = YM2151Driver;
		d.setTone(0, 52, 0);
		d.setLFOFreq(90);
		d.setWaveForm(64);
		d.setAlgorithm(0, 0<<4);
		d.setOpActive(0, 0, 0<<6);
		d.setOpActive(0, 1, 1<<6);
		d.setOpActive(0, 2, 0<<6);
		d.setOpActive(0, 3, 1<<6);

		d.setOpVolume(0, 1, 32);
		d.setDet1(0, 1, 1<<4);
		d.setMul(0,  1, 0<<3);
		d.setDet2(0, 1, 0<<5);
		// d.setAMSenseEn(0, 0, 127);
		// d.setAMSenseEn(0, 1, 1<<6);
		// d.setAMSenseEn(0, 2, 127);
		// d.setAMSenseEn(0, 3, 1<<6);

		// d.setAMSense(0, 3<<5);
		d.setPMSense(0, 127);
		d.setAmpDepth(0);
		d.setPhaseDepth(1);

		d.noteOff(0);
		d.noteOn(0);
		while(1)
		{
		}
	}

	//SysEx:
	void handle_sysex(uint8_t* a, unsigned sizeofsysex){
		SysExHandler.handleSysEx(a, sizeofsysex);
		delete a; // we don't need the sysex Array anymore
	}

	void handle_CC(uint8_t channel, uint8_t number, uint8_t value){
		channel = channel - 1;
		CCHandler.handleCC(channel, number, value);
	}
	void handle_PC(uint8_t channel, uint8_t number){

	}

	void handle_NoteOn(uint8_t channel, uint8_t note, uint8_t value){
		channel = channel - 1;
		NotePool.handleNote(true, channel, note, value);
	}

	void handle_NoteOff(uint8_t channel, uint8_t note, uint8_t value){
		channel = channel - 1;
		NotePool.handleNote(false, channel, note, value);
	}


