#include "core/MidiHandler.h"
#include <memory>

MidiHandler::MidiHandler() : nextHandler(nullptr) {}

void MidiHandler::setNextHandler(shared_ptr<MidiHandler> handler) { nextHandler = handler; }

void MidiHandler::handle(MidiMsg &event) {
  process(event);
  if (!event.isHandled && nextHandler) {
    nextHandler->handle(event);
  }
}
