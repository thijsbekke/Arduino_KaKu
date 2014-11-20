/*
 * NewRemoteSwitch library v1.1.0 (20130601) made by Randy Simons http://randysimons.nl/
 * See KaKuTransmitter.h for details.
 *
 * License: GPLv3. See license.txt
 */

#include "KaKuTransmitter.h"
#include <JeeLib.h>
 
KaKuTransmitter::KaKuTransmitter(unsigned long address, unsigned int periodusec, byte repeats) {
    _address = address;
    _periodusec = periodusec;
    _repeats = (1 << repeats) - 1; // I.e. _repeats = 2^repeats - 1
}

void KaKuTransmitter::sendGroup(boolean switchOn) {
    for (int8_t i = _repeats; i >= 0; i--) {
        _sendStartPulse();

        _sendAddress();

        // Do send group bit
        _sendBit(true);

        // Switch on | off
        _sendBit(switchOn);

        // No unit. Is this actually ignored?..
        _sendUnit(0);

        _sendStopPulse();
    }
}

void KaKuTransmitter::sendUnit(byte unit, boolean switchOn) {
    for (int8_t i = _repeats; i >= 0; i--) {
        _sendStartPulse();

        _sendAddress();

        // No group bit
        _sendBit(false);

        // Switch on | off
        _sendBit(switchOn);

        _sendUnit(unit);

        _sendStopPulse();
    }
}

void KaKuTransmitter::sendDim(byte unit, byte dimLevel) {
    for (int8_t i = _repeats; i >= 0; i--) {
        _sendStartPulse();

        _sendAddress();

        // No group bit
        _sendBit(false);

        // Switch type 'dim'
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec);
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec);

        _sendUnit(unit);

        for (int8_t j=3; j>=0; j--) {
           _sendBit(dimLevel & 1<<j);
        }

        _sendStopPulse();
    }
}

void KaKuTransmitter::_sendStartPulse(){
    rf12_onOff(1);
    delayMicroseconds(_periodusec);
    rf12_onOff(0);
    delayMicroseconds(_periodusec * 10 + (_periodusec >> 1)); // Actually 10.5T insteat of 10.44T. Close enough.
}

void KaKuTransmitter::_sendAddress() {
    for (int8_t i=25; i>=0; i--) {
       _sendBit((_address >> i) & 1);
    }
}

void KaKuTransmitter::_sendUnit(byte unit) {
    for (int8_t i=3; i>=0; i--) {
       _sendBit(unit & 1<<i);
    }
}

void KaKuTransmitter::_sendStopPulse() {
    rf12_onOff(1);
    delayMicroseconds(_periodusec);
    rf12_onOff(0);
    delayMicroseconds(_periodusec * 40);
}

void KaKuTransmitter::_sendBit(boolean isBitOne) {
    if (isBitOne) {
        // Send '1'
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec * 5);
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec);
    } else {
        // Send '0'
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec);
        rf12_onOff(1);
        delayMicroseconds(_periodusec);
        rf12_onOff(0);
        delayMicroseconds(_periodusec * 5);
    }
}
