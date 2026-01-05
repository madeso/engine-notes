// drug range effect.
// Value fades over time
// boost decreases after use
// Boost goes back up after time
// use doesn't add to full range so mure use can give you more
// Take too much and get overdose

#include <cassert>

struct Character;

struct Drug
{
    // how quickly the drug is removed from your system, per second
    float fade_rate = 0.1f;

    // how quickly boost recovers per second
    float boost_recovery = 0.05f;
    float boost_min = 0.0f;

    // how much boost to loose per dosage
    float boost_factor = 0.2f;

    // overdose decay rate
    float overdose_decay = 0.05f;
    float overdose_max = 1.0f;
    
    virtual void apply_effect(Character* c, float strength) = 0;
    virtual void apply_time(Character* c, float desire, float shiver) = 0;
};

struct Usage
{
    // how much drug is in your system
    float fade = 0.0f;
    // how much your body has adopted to the drug, 
    float boost = 1.0f;
    float overdose = 0.0f;
    Drug* drug;

    void apply(Character* c)
    {
        assert(drug);

        const auto effect = fade * boost;
        drug->apply_effect(c, effect);
    }

    void take(float dosage)
    {
        assert(drug);

        // Increase fade and overdose by dosage, but clamp fade to 1.0
        fade += dosage;
        if (fade > 1.0f) fade = 1.0f;

        overdose += dosage;
        // todo(Gustav): check of overdose

        // Decrease boost with each use, but not below 0
        boost -= dosage * drug->boost_factor;
        if (boost < drug->boost_min) boost = drug->boost_min;
    }

    void update(Character* c, float dt)
    {
        if (!drug) return;

        // fade
        fade -= drug->fade_rate * dt;
        if (fade < 0.0f) fade = 0.0f;

        // boost
        constexpr float max_boost = 1.0f;
        boost += drug->boost_recovery * dt;
        if (boost > max_boost) boost = max_boost;

        // overdose
        overdose -= drug->overdose_decay * dt;
        if (overdose < 0.0f) overdose = 0.0f;

        // drug has left system but body hasn't adapted to no drugs => desire
        const float desire = fade > 0.0f ? 0.0f : boost - max_boost;

        // how close are you to overdosing?
        const float shiver = 0.0f;

        drug->apply_time(c, desire, shiver);
    }
};
