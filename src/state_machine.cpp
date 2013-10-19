#include "state_machine.h"

unsigned int StateMachineBase::getState() const {
    return (unsigned int)m_internalState;
}

void StateMachineBase::setState(unsigned char state) {
    m_internalState = state;
}

bool StateMachineBase::fatalError() const {
    return m_internalState == FATAL_STATE;
}
