#pragma once
struct animation;
class CResolver
{
private:
    float GetLeftYaw(IBasePlayer*);
    float GetRightYaw(IBasePlayer*);
    bool TargetSide(IBasePlayer*);
    void DetectSide(IBasePlayer*, int*);
    void FixPitch(IBasePlayer* pEnt);
    bool TargetJitter(IBasePlayer*, bool);
    bool DoesHaveJitter(IBasePlayer*, int*);
    int FreestandSide[64];
public:
    float GetAngle(IBasePlayer*);
    float GetForwardYaw(IBasePlayer*);
    float GetBackwardYaw(IBasePlayer*);
    void StoreAntifreestand();
    void Do3(IBasePlayer* player);
    void Do5(IBasePlayer* player);
    void Do2(IBasePlayer* player);
    void Do(IBasePlayer* player);
};
extern CResolver* resolver;
extern std::string ResolverMode[65];

///////////////////