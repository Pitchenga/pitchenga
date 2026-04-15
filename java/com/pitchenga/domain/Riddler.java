package com.pitchenga.domain;

import com.pitchenga.Pitchenga;

import java.util.*;
import java.util.function.Function;

import static com.pitchenga.domain.Pitch.*;
import static com.pitchenga.Pitchenga.CHROMATIC_SCALE;
import static com.pitchenga.Pitchenga.transposeScale;
import static com.pitchenga.domain.Tone.*;

public enum Riddler {
    //    FuguesOrdered("All fugues ordered",
//            new Pitch[][][]{{{Do3, Ra3, Re3, Me3, Mi3, Fa3, Fi3, So3, Le3, La3, Se3, Si3, Do4,π}}}, Pitchenga::ordered, null, null, new int[0]),
    Rec("Rec", new Pitch[][][]{{{Do4, Ti3, Te3, La3, Le3, So3, Fi3, Fa3, Mi3, Me3, Re3, Ra3, Do3,}}},
            Pitchenga::ordered, null, null, new int[0], new String[]{""}),
    FuguesOrdered("All fugues ordered",
            new Pitch[][][]{{Arrays.stream(Fugue.values()).map(fugue -> fugue.pitch).toArray(Pitch[]::new)}}, Pitchenga::ordered, null, null, new int[0], new String[]{"Do", "Ra", "Re", "Me", "Mi", "Fa", "Fi", "So", "Le", "La", "Te", "Ti"}),
    ChromaticOneOctave("Chromatic - octave 4",
            new Pitch[][][]{{Pitchenga.CHROMATIC_SCALE}}, Pitchenga::shuffle, new Integer[0], null, new int[0], new String[]{"Do", "Ra", "Re", "Me", "Mi", "Fa", "Fi", "So", "Le", "La", "Te", "Ti"}),
    Chromatic("Chromatic - main octaves",
            new Pitch[][][]{{Pitchenga.CHROMATIC_SCALE}}, Pitchenga::shuffle, null, null, new int[0], new String[]{"Do", "Ra", "Re", "Me", "Mi", "Fa", "Fi", "So", "Le", "La", "Te", "Ti"}),
    //    Step51("51", new Pitch[][][]{
//            multiply(new Pitch[]{Do2, Me2, Fi2, La2, Do3,}, 110),
//            multiply(Sets.octavesDo2ToDo7, 10),
//            {{Non}},},
//            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
//            new int[]{0, 1, 0}),
//    Step52("52", new Pitch[][][]{
//            multiply(new Pitch[]{Do2, Me2, Fa2, Fa2, Fa2, Fi2, Fi2, Fi2, La2, Si2, Si2, Si2, Si2, Do3,}, 60),
//            multiply(Sets.octavesDo2ToDo7, 9),
//            {{Non}},},
//            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
//            new int[]{0, 1, 0}),
//    Step53("53", new Pitch[][][]{
//            multiply(new Pitch[]{Do2, Ra2, Ra2, Ra2, Me2, Fa2, Fi2, So2, So2, So2, La2, Si2, Do3,}, 500),
//            multiply(Sets.octavesDo2ToDo7, 500),
//            {{Non}},},
//            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
//            new int[]{0, 1, 0}),
    Step54("54", new Pitch[][][]{
            multiply(50, new Pitch[]{Do2, Ra2, Me2, Mi2, Mi2, Mi2, Mi2, Fa2, Fi2, So2, La2, Te2, Te2, Te2, Te2, Ti2, Do3,}),
            multiply(500, Scale.octavesDo2ToDo7),
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            //fixme: Insturments
            new int[]{0, 0, 0}, new String[]{"Do", "Ra", "Re", "Me", "Mi", "Fa", "Fi", "So", "Le", "La", "Te", "Ti"}),
    BassOct2MiSe(null, new Pitch[][][]{
            multiply(400, new Pitch[]{Do2, Ra2, Me2, Mi2, Mi2, Mi2, Mi2, Fa2, Fi2, So2, La2, Te2, Te2, Te2, Te2, Ti2, Do3,}),
            multiply(50, transposeScale(Scale.Do3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.So3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Re3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.La3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Mi3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Ti3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Fi3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Ra3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Le3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Me3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Te3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Fa3Maj.getScale(), -1, 0)),
            multiply(300, Scale.octavesDo2ToDo7),
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
            new String[]{"Mi/Te", "Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa", "Piano", "End"}),
    //fixme multi-oct voice hints
    BassOct2ReLe(null, new Pitch[][][]{
            multiply(400, new Pitch[]{Do2, Ra2, Re2, Re2, Re2, Re2, Re2, Re2, Me2, Mi2, Fa2, Fi2, So2, Le2, Le2, Le2, Le2, Le2, Le2, La2, Te2, Ti2, Do3,}),
            multiply(50, transposeScale(Scale.Do3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.So3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Re3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.La3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Mi3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Ti3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Fi3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Ra3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Le3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Me3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Te3Maj.getScale(), -1, 0)),
            multiply(50, transposeScale(Scale.Fa3Maj.getScale(), -1, 0)),
            Scale.DoMajWide,
            Scale.SoMajWide,
            Scale.ReMajWide,
            Scale.LaMajWide,
            Scale.MiMajWide,
            Scale.TiMajWide,
            Scale.FiMajWide,
            Scale.RaMajWide,
            Scale.LeMajWide,
            Scale.MeMajWide,
            Scale.TeMajWide,
            Scale.FaMajWide,
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            new int[]{0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            new String[]{"Re/Le",
                    "Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa",
                    "Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa", "End"}),
    BassAll(null, new Pitch[][][]{
            merge(70, transposeScale(Scale.Do3Maj.getScale(), 0, 0), new Pitch[]{Ti3, Ti3, Ti3}),
            merge(30, transposeScale(Scale.Do3Maj.getScale(), -1, 0)),
            Scale.DoMajWide,
            merge(70, transposeScale(Scale.So3Maj.getScale(), 0, 0), new Pitch[]{Fi3, Fi3, Fi3}),
            merge(30, transposeScale(Scale.So3Maj.getScale(), -1, 0)),
            Scale.SoMajWide,
            merge(70, transposeScale(Scale.Re3Maj.getScale(), 0, 0), new Pitch[]{Ra3, Ra3, Ra3}),
            merge(30, transposeScale(Scale.Re3Maj.getScale(), -1, 0)),
            Scale.ReMajWide,
            merge(70, transposeScale(Scale.La3Maj.getScale(), 0, 0), new Pitch[]{Le3, Le3, Le3}),
            merge(30, transposeScale(Scale.La3Maj.getScale(), -1, 0)),
            Scale.LaMajWide,
            merge(70, transposeScale(Scale.Mi3Maj.getScale(), 0, 0), new Pitch[]{Me3, Me3, Me3}),
            merge(30, transposeScale(Scale.Mi3Maj.getScale(), -1, 0)),
            Scale.MiMajWide,
            merge(70, transposeScale(Scale.Ti3Maj.getScale(), 0, 0), new Pitch[]{Te3, Te3, Te3}),
            merge(30, transposeScale(Scale.Ti3Maj.getScale(), -1, 0)),
            Scale.TiMajWide,
            merge(70, transposeScale(Scale.Fi3Maj.getScale(), 0, 0), new Pitch[]{Fa3, Fa3, Fa3}),
            merge(30, transposeScale(Scale.Fi3Maj.getScale(), -1, 0)),
            Scale.FiMajWide,
            merge(70, transposeScale(Scale.Ra3Maj.getScale(), 0, 0), new Pitch[]{Do3, Do3, Do3}),
            merge(30, transposeScale(Scale.Ra3Maj.getScale(), -1, 0)),
            Scale.RaMajWide,
            merge(70, transposeScale(Scale.Le3Maj.getScale(), 0, 0), new Pitch[]{So3, So3, So3}),
            merge(30, transposeScale(Scale.Le3Maj.getScale(), -1, 0)),
            Scale.LeMajWide,
            merge(70, transposeScale(Scale.Me3Maj.getScale(), 0, 0), new Pitch[]{Re3, Re3, Re3}),
            merge(30, transposeScale(Scale.Me3Maj.getScale(), -1, 0)),
            Scale.MeMajWide,
            merge(70, transposeScale(Scale.Te3Maj.getScale(), 0, 0), new Pitch[]{La3, La3, La3}),
            merge(30, transposeScale(Scale.Te3Maj.getScale(), -1, 0)),
            Scale.TeMajWide,
            merge(70, transposeScale(Scale.Fa3Maj.getScale(), 0, 0), new Pitch[]{Mi3, Mi3, Mi3}),
            merge(30, transposeScale(Scale.Fa3Maj.getScale(), -1, 0)),
            Scale.FaMajWide,
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            //fixme: Use macro groups and remove duplication
            new int[]{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1},
            new String[]{"Do", "Do", "Do", "So", "So", "So", "Re", "Re", "Re", "La", "La", "La", "Mi", "Mi", "Mi", "Ti", "Ti", "Ti", "Fi", "Fi", "Fi", "Ra", "Ra", "Ra", "Le", "Le", "Le", "Me", "Me", "Me", "Te", "Te", "Te", "Fa", "Fa", "Fa", "End",}),
    Bass(null, new Pitch[][][]{
            multiply(300, new Pitch[]{Do1, Do1, Me1, Me1, Fi1, La1,}),
            multiply(300, filter(new Pitch[][]{transposeScale(CHROMATIC_SCALE, -1, 0)}, Do, Me, Fi, La)),
            multiply(300, filter(new Pitch[][]{transposeScale(CHROMATIC_SCALE, 0, 0)}, Do, Me, Fi, La)),
            multiply(300, filter(Scale.octavesDo2ToDo7, Do, Me, Fi, La)),
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            new int[]{0, 0, 0, 1, 0,},
            new String[]{"Bass1", "Bass2", "Bass3", "Piano", "End",}),
    BassScales(null, new Pitch[][][]{
            multiply(40, transposeScale(Scale.Do3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.So3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Re3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.La3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Mi3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Ti3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Fi3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Ra3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Le3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Me3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Te3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Fa3Maj.getScale(), -2, 0)),
            multiply(40, transposeScale(Scale.Do3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.So3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Re3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.La3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Mi3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Ti3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Fi3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Ra3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Le3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Me3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Te3Maj.getScale(), -1, 0)),
            multiply(40, transposeScale(Scale.Fa3Maj.getScale(), -1, 0)),
            Scale.DoMajWide,
            Scale.SoMajWide,
            Scale.ReMajWide,
            Scale.LaMajWide,
            Scale.MiMajWide,
            Scale.TiMajWide,
            Scale.FiMajWide,
            Scale.RaMajWide,
            Scale.LeMajWide,
            Scale.MeMajWide,
            Scale.TeMajWide,
            Scale.FaMajWide,
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,},
            new String[]{"Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa",
                    "Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa",
                    "Do", "So", "Re", "La", "Mi", "Ti", "Fi", "Ra", "Le", "Me", "Te", "Fa",
                    "End",}),
    BassScales2(null, new Pitch[][][]{
            multiply(30, transposeScale(Scale.Do3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Do3Maj.getScale(), -1, 0)),
            Scale.DoMajWide,
            multiply(30, transposeScale(Scale.So3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.So3Maj.getScale(), -1, 0)),
            Scale.SoMajWide,
            multiply(30, transposeScale(Scale.Re3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Re3Maj.getScale(), -1, 0)),
            Scale.ReMajWide,
            multiply(30, transposeScale(Scale.La3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.La3Maj.getScale(), -1, 0)),
            Scale.LaMajWide,
            multiply(30, transposeScale(Scale.Mi3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Mi3Maj.getScale(), -1, 0)),
            Scale.MiMajWide,
            multiply(30, transposeScale(Scale.Ti3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Ti3Maj.getScale(), -1, 0)),
            Scale.TiMajWide,
            multiply(30, transposeScale(Scale.Fi3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Fi3Maj.getScale(), -1, 0)),
            Scale.FiMajWide,
            multiply(30, transposeScale(Scale.Ra3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Ra3Maj.getScale(), -1, 0)),
            Scale.RaMajWide,
            multiply(30, transposeScale(Scale.Le3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Le3Maj.getScale(), -1, 0)),
            Scale.LeMajWide,
            multiply(30, transposeScale(Scale.Me3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Me3Maj.getScale(), -1, 0)),
            Scale.MeMajWide,
            multiply(30, transposeScale(Scale.Te3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Te3Maj.getScale(), -1, 0)),
            Scale.TeMajWide,
            multiply(30, transposeScale(Scale.Fa3Maj.getScale(), -2, 0)),
            multiply(30, transposeScale(Scale.Fa3Maj.getScale(), -1, 0)),
            Scale.FaMajWide,
            {{Non}},},
            pitchenga -> pitchenga.shuffleGroupSeries(false, true), new Integer[0], null,
            new int[]{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,},
            new String[]{"Do", "Do", "Do", "So", "So", "So", "Re", "Re", "Re", "La", "La", "La", "Mi", "Mi", "Mi", "Ti", "Ti", "Ti", "Fi", "Fi", "Fi", "Ra", "Ra", "Ra", "Le", "Le", "Le", "Me", "Me", "Me", "Te", "Te", "Te", "Fa", "Fa", "Fa", "End",}),
    ;

    public final String[] messages;

    private static Pitch[][] filter(Pitch[][] pitches, Tone... tones) {
        Pitch[][] result = new Pitch[pitches.length][];
        Set<Tone> toneSet = new HashSet<>(Arrays.asList(tones));
        for (int i = 0; i < result.length; i++) {
            Pitch[] sourceRow = pitches[i];
            List<Pitch> resultRow = new ArrayList<>(sourceRow.length);
            for (Pitch pitch : sourceRow) {
                if (toneSet.contains(pitch.tone)) {
                    resultRow.add(pitch);
                }
            }
            result[i] = resultRow.toArray(new Pitch[0]);
        }
        return result;
    }

    @SuppressWarnings("SameParameterValue")
    private static Pitch[][] multiply(int count, Pitch[] pitches) {
        int times = count / pitches.length;
        Pitch[][] result = new Pitch[times][];
        Arrays.fill(result, pitches);
        return result;
    }

    @SuppressWarnings("SameParameterValue")
    private static Pitch[][] multiply(int count, Pitch[][] rows) {
        int size = 0;
        List<Pitch[]> result = new LinkedList<>();
        while (size <= count) {
            for (Pitch[] row : rows) {
                size += row.length;
            }
            Collections.addAll(result, rows);
        }
        return result.toArray(new Pitch[0][]);
    }

    @SuppressWarnings("SameParameterValue")
    private static Pitch[][] merge(int count, Pitch[]... rows) {
        List<Pitch> merged = new LinkedList<>();
        for (Pitch[] row : rows) {
            merged.addAll(Arrays.asList(row));
        }
        return multiply(count, merged.toArray(new Pitch[0]));
    }

    private final String name;
    public final Pitch[][][] scale;
    public final Function<Pitchenga, List<Pitch>> riddle;
    public final Integer[] octaves;
    public final Hinter hinter;
    public final int[] instruments;

    Riddler(String nameOverride, Pitch[][][] scale, Function<Pitchenga, List<Pitch>> riddle, Integer[] octaves, Hinter hinter, int[] instruments, String[] messages) {
        if (nameOverride == null) {
            nameOverride = name();
        }
        this.name = nameOverride;
        this.scale = scale;
        this.riddle = riddle;
        this.octaves = octaves;
        this.hinter = hinter;
        this.instruments = instruments;
        this.messages = messages;
    }

    public String toString() {
        return name;
    }

}