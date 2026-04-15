package com.pitchenga.domain;

import com.pitchenga.Pitchenga;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.pitchenga.domain.Duration.*;
import static com.pitchenga.domain.Pitch.*;

public enum Fugue {

    Do(Do4, new Pitch[]{Do2, Do3, Do4, Do5, Do6, Do7}, eight, Do4, eight, Do4, four),
    Ra(Ra4, new Pitch[]{Ra1, Ra2, Ra3, Ra4, Ra5, Ra6}, four, Do4, four),
    Re(Re4, new Pitch[]{Re1, Re2, Re3, Re4, Re5, Re6}, four, Do4, four),
    Me(Me4, new Pitch[]{Me1, Me2, Me3, Me4, Me5, Me6}, four, sixteen, Do4, four),
    Mi(Mi4, new Pitch[]{Mi1, Mi2, Mi3, Mi4, Mi5, Mi6}, four, Re4, sixteen, Do4, four),
    Fa(Fa4, new Pitch[]{Fa1, Fa2, Fa3, Fa4, Fa5, Fa6}, eight, Mi4, sixteen, Re4, sixteen, Do4, four),
    Fi(Fi4, new Pitch[]{Fi1, Fi2, Fi3, Fi4, Fi5, Fi6}, eight, So4, sixteen, Do5, four),
    So(So4, new Pitch[]{So1, So2, So3, So4, So5, So6}, eight, La4, sixteen, Ti4, sixteen, Do5, four),
    Le(Le4, new Pitch[]{Le1, Le2, Le3, Le4, Le5, Le6}, four, So4, sixteen, Do5, four),
    La(La4, new Pitch[]{La1, La2, La3, La4, La5, La6}, four, Ti4, sixteen, Do5, four),
    Te(Te4, new Pitch[]{Te1, Te2, Te3, Te4, Te5, Te6}, four, sixteen, Do5, four),
    Ti(Ti4, new Pitch[]{Ti1, Ti2, Ti3, Ti4, Ti5, Ti6}, four, Do5, four),
    DoDo(Do5, Do.multiPitch, eight, Do5, eight, Do5, four),
    ;

    public final Pitch pitch;
    public final Pitch[] multiPitch;
    public final int[] multiPitchMidis;
    public final Object[] tune;
    public final Object[][] intervals;

    Fugue(Pitch pitch, Pitch[] multiPitch, Object... coda) {
        this.pitch = pitch;
        this.multiPitch = multiPitch;
        this.multiPitchMidis = new int[multiPitch.length];
        for (int i = 0; i < multiPitch.length; i++) {
            multiPitchMidis[i] = multiPitch[i].midi;
        }
        List<Object> tune = new ArrayList<>(coda.length + 1);
        tune.add(pitch);
        tune.addAll(Arrays.asList(coda));
        this.tune = tune.toArray();
        Pitch theDo = (Pitch) coda[coda.length - 2];
        Tone[] tones = Tone.values();
        intervals = new Object[tones.length][];
        for (Tone tone : tones) {
            Object[] interval = new Object[4];
            this.intervals[tone.ordinal()] = interval;
            Pitch end = Pitchenga.transposePitch(theDo, 0, tone.ordinal());
            interval[0] = pitch;
            interval[1] = eight;
            interval[2] = end;
            interval[3] = eight;
        }
    }

}