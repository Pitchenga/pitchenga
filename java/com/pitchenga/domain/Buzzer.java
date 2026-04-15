package com.pitchenga.domain;

import com.pitchenga.Pitchenga;

import java.util.function.Function;

import static com.pitchenga.domain.Duration.*;
import static com.pitchenga.domain.Tone.Do;

public enum Buzzer {
    Tune("Riddle mnemonic tune", Pitchenga::transposeTune),
    Tone("Riddle tone", pitch -> new Object[]{pitch, sixteen}),
//    Tone("Riddle tone", pitch -> new Object[]{pitch, sixteen}),
    //    Tone("Riddle tone", pitch -> new Object[]{eight, pitch, sixteen}),
    ShortToneAndLongPause("Riddle shorter tone with longer pause (for acoustic instruments)", pitch -> new Object[]{pitch, thirtyTwo, four, four}), //Otherwise the game plays with itself through the microphone by picking up the "tail". This could probably be improved with a shorter midi decay.
    ToneAndDo("Riddle tone and Do", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{pitch.tone.getFugue().pitch, Do.getFugue().pitch, sixteen, four})),
    ;
    private final String name;
    public final Function<Pitch, Object[]> buzz;

    Buzzer(String name, Function<Pitch, Object[]> buzz) {
        this.name = name;
        this.buzz = buzz;
    }

    @Override
    public String toString() {
        return name;
    }
}