#include "state_machine.h"

unsigned char StateMachineBase::getState() const {
    return m_internalState;
}

void StateMachineBase::setState(unsigned char state) {
    m_internalState = state;
}

bool StateMachineBase::fatalError() const {
    return m_internalState == FATAL_STATE;
}
