#include "Arduino.h"
#include "ServerLib.h"
#include "SoftwareSerial.h"

/*
ServerLib::ServerLib(int rx, int tx, String naam, String game) : _bt(SoftwareSerial(rx, tx)) {
	_naam = naam;
	_game = game;

	//SoftwareSerial bt(rx, tx);
	//_bt = &bt;
	_bt.begin(9600);
}
*/

ServerLib::ServerLib(SoftwareSerial& serial, String naam, String game) : _bt(serial) {
	_naam = naam;
	_game = game;
}

void ServerLib::updateLoop() {
	if(_bt.available() > 0) {
		int data = _bt.peek();

		if(data < 0) { // arduino wtf zijn we aan het doen
			Serial.print("hallo ik ben arduino in welkom bij jackass: ");
			Serial.println(String(data, DEC) + " == " + String(_bt.read(), DEC));
			return; // vraag me niet waarom maar dit werkt want ik weet het ook niet
		}

		bool found = true;

		if(data == 'h') {
			_bt.print("{\"COMMAND\":\"HANDSHAKE\", \"NAME\":\"");
			_bt.print(_naam);
			_bt.print("\", \"GAME\":\"");
			_bt.print(_game);
			_bt.println("\"}");
		} else if(data == 'b') {
			_started = true;
		} else if(data == 's') {
			_started = false;
		} else if(data == 'p') {
			_bt.println("{\"COMMAND\":\"PING\"}");
			_connected = true;
			_lastPing = millis();
		} else if(data == 'r') {
			if(_ready) {
				_bt.println("{\"COMMAND\":\"STATE\", \"VALUE\":\"READY\"}");
			} else {
				_bt.println("{\"COMMAND\":\"STATE\", \"VALUE\":\"NOTREADY\"}");
			}
		} else {
			found = false;
		}

		if(found) _bt.read();
	}

	if(_connected && ((millis() - _lastPing) > 1000)) {
		_connected = false;
	}
}

void ServerLib::setReadyState(bool state) {
	if(state == _ready) return;
	_ready = state;

	if(_ready) {
		_bt.println("{\"COMMAND\":\"STATE\", \"VALUE\":\"READY\"}");
	} else {
		_bt.println("{\"COMMAND\":\"STATE\", \"VALUE\":\"NOTREADY\"}");
	}
}

bool ServerLib::hasGameStarted() {
	return _started;
}

bool ServerLib::isConnected() {
	return _connected;
}

void ServerLib::scorePoint() {
	_bt.println("{\"COMMAND\":\"POINT\"}");
}