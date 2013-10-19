/* State machine base class for nearly all objects; this helps communicate behavioral
 * information between objects.
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_


/* Define/declare all state here for each individual module. */
#define FATAL_STATE     127
#define INIT_STATE      0

/* Dodgeball engine states */
#define DGBENG_RUNNING  1

/* DodgeballNode specific states */
#define DGDBL_ACTIVE    10
#define DGDBL_INACTIVE  11

/* Player-specific states */
#define PLAYER_ACTIVE   20
#define PLAYER_OUT      21

class StateMachineBase {
    public:
        virtual unsigned int getState() const;
        virtual void setState(unsigned char state);
        virtual bool fatalError() const;
    
    private:
        unsigned char m_internalState;
};

#endif
