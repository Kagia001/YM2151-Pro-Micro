#include "Arduino.h"
/**
*	YM2151 - Chip Controller Software for ARDUINO
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

#include "NotePool.h"
#include "YM2151Driver.h"


void NotePoolClass::init()
{
	for (byte i = 0; i < 8; i++){
		NotePoolClass::notes[i] = 0xFF;
		NotePoolClass::statusTimestamp[i] = 0L;
	}
	NotePoolClass::resetNotes();
	mode = false;
}


void NotePoolClass::handleNote(bool on, uint8_t channel, uint8_t pitch, uint8_t velocity){
	if (NotePoolClass::mode){ // monophonic mode
		if (on){	
			NotePoolClass::notes[channel] = pitch;
			YM2151Driver.setTone(channel, pitch, 0);
			YM2151Driver.setVelocity(channel, velocity);
			YM2151Driver.noteOff(channel);
			YM2151Driver.noteOn(channel);
		}
		else if (NotePoolClass::notes[channel] == pitch){
			NotePoolClass::notes[channel] = 0xFF;
			YM2151Driver.noteOff(channel);
		}
	}
	else{

		uint8_t ymChannel = 0xFF;
		if (on){

			ymChannel = findNote(pitch);
			if (ymChannel == 0xFF){ // don’t start same note twice before stopping it
				byte ch = getPreferredChannel();

				notes[ch] = pitch;
				statusTimestamp[ch] = millis();
				YM2151Driver.setTone(ch, pitch, 0);
			  YM2151Driver.setVelocity(ch, velocity);
				YM2151Driver.noteOff(ch);
				YM2151Driver.noteOn(ch);
			}
		}
		else{
			ymChannel = findNote(pitch);
			if (ymChannel != 0xFF){
				notes[ymChannel] = 0xFF;
				statusTimestamp[ymChannel] = millis();
				YM2151Driver.noteOff(ymChannel);
			}
		}
	}
}

	/**
	*	false -> Poly
	*	true -> Channel
	*/
void NotePoolClass::setMode(bool channel){
	if (channel != mode){
		mode = channel;
		resetNotes();
	}
}

bool NotePoolClass::getMode(){
	return mode;
}


void NotePoolClass::resetNotes(){
	for (uint8_t i = 0; i < 8; i++){
		notes[i] = 0xFF;
		YM2151Driver.noteOff(i);
	}
}

uint8_t NotePoolClass::findNote(uint8_t note){
	for (uint8_t i = 0; i < 8; i++){
		if (notes[i] == note){
			return i;
		}
	}
	return 0xFF;
}

// Find a good channel for the next note.
// Algorithm:
// If there is an OFF channel
//    take the one that has been off longest.
//    It might still be producing sound based on release envelope
// If all channels are currently ON
//		take the one that has been on longest.
//		meaning that all 7 other notes have been started after that one

uint8_t NotePoolClass::getPreferredChannel(){
	uint8_t prefChannel = 0;
	bool prefChannelIsOff = (notes[0] == 0xFF);
	unsigned long prefChannelTimestamp = statusTimestamp[0];

	for (uint8_t i = 1; i < 8; i++){
		// This channel is better if the status was changed earlier, or it is off and the current best is on.
		if (statusTimestamp[i] < prefChannelTimestamp || (!prefChannelIsOff && notes[i]==0xFF)) {
			prefChannel = i;
			prefChannelTimestamp = statusTimestamp[i];
			prefChannelIsOff = notes[i] == 0xFF;
		}
	}
	return prefChannel;
}


NotePoolClass NotePool;

