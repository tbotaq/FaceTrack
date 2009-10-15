#if !defined BarrettHand_h
#define BarrettHand_h

class BarrettHand {

    enum FingerMask {F1=0x1,F2=0x2,F3=0x4,Spread=0x8,Grasp=0x7,AllFingers=0xF};

    // "HI" command. Must be called before any move commands are issued.
    bool HandInitialize();
};

#endif