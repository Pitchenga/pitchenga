package com.pitchenga.domain;

import com.pitchenga.Pitchenga;

import java.util.function.Function;

import static com.pitchenga.domain.Duration.*;
import static com.pitchenga.domain.Tone.*;

public enum Ringer {
    None("Ring nothing", pitch -> new Object[]{thirtyTwo}),
    Tune("Ring mnemonic tune", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().tune)),
    Tone("Ring tone", pitch -> new Object[]{pitch, eight, eight}),
    JustDo("Ring Do", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Do.getFugue().pitch, eight, four})),
    JustRa("Ring Ra", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Ra.getFugue().pitch, eight, four})),
    JustRe("Ring Re", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Re.getFugue().pitch, eight, four})),
    JustMe("Ring Me", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Me.getFugue().pitch, eight, four})),
    JustMi("Ring Mi", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Mi.getFugue().pitch, eight, four})),
    JustFa("Ring Fa", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Fa.getFugue().pitch, eight, four})),
    JustFi("Ring Fi", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Fi.getFugue().pitch, eight, four})),
    JustSo("Ring So", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{So.getFugue().pitch, eight, four})),
    JustLe("Ring Le", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Le.getFugue().pitch, eight, four})),
    JustLa("Ring La", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{La.getFugue().pitch, eight, four})),
    JustSe("Ring Te", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Te.getFugue().pitch, eight, four})),
    JustSi("Ring Ti", pitch -> Pitchenga.transposeFugue(pitch, new Object[]{Ti.getFugue().pitch, eight, four})),
    ToneAndDo("Ring tone and Do", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Do.ordinal()])),
    ToneAndRa("Ring tone and Ra", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Ra.ordinal()])),
    ToneAndRe("Ring tone and Re", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Re.ordinal()])),
    ToneAndMe("Ring tone and Me", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Me.ordinal()])),
    ToneAndMi("Ring tone and Mi", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Mi.ordinal()])),
    ToneAndFa("Ring tone and Fa", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Fa.ordinal()])),
    ToneAndFi("Ring tone and Fi", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Fi.ordinal()])),
    ToneAndSo("Ring tone and So", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[So.ordinal()])),
    ToneAndLe("Ring tone and Le", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Le.ordinal()])),
    ToneAndLa("Ring tone and La", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[La.ordinal()])),
    ToneAndSe("Ring tone and Te", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Te.ordinal()])),
    ToneAndSi("Ring tone and Ti", pitch -> Pitchenga.transposeFugue(pitch, pitch.tone.getFugue().intervals[Ti.ordinal()])),
    ;
    private final String name;
    public final Function<Pitch, Object[]> ring;

    Ringer(String name, Function<Pitch, Object[]> ring) {
        this.name = name;
        this.ring = ring;
    }

    @Override
    public String toString() {
        return name;
    }
}