package com.pitchenga.domain;

import static com.pitchenga.domain.Pitch.*;

public enum Scale {

    Do3Maj(new Pitch[]{Do3, Re3, Mi3, Fa3, So3, La3, Ti3, Do4,}),
    Ra3Maj(new Pitch[]{Ra3, Me3, Fa3, Fi3, Le3, Te3, Do4, Do3,}),
    Re3Maj(new Pitch[]{Re3, Mi3, Fi3, So3, La3, Ti3, Ra3, Re3,}),
    Me3Maj(new Pitch[]{Me3, Fa3, So3, Le3, Te3, Do4, Re3, Do3,}),
    Mi3Maj(new Pitch[]{Mi3, Fi3, Le3, La3, Ti3, Ra3, Me3, Mi3,}),
    Fa3Maj(new Pitch[]{Fa3, So3, La3, Te3, Do4, Re3, Mi3, Do3,}),
    Fi3Maj(new Pitch[]{Fi3, Le3, Te3, Ti3, Ra3, Me3, Fa3, Fi3,}),
    So3Maj(new Pitch[]{So3, La3, Ti3, Do4, Re3, Mi3, Fi3, Do3,}),
    Le3Maj(new Pitch[]{Le3, Te3, Do4, Ra3, Me3, Fa3, So3, Do3,}),
    La3Maj(new Pitch[]{La3, Ti3, Ra3, Re3, Mi3, Fi3, Le3, La3,}),
    Te3Maj(new Pitch[]{Te3, Do4, Re3, Me3, Fa3, So3, La3, Do3,}),
    Ti3Maj(new Pitch[]{Ti3, Ra3, Me3, Mi3, Fi3, Le3, Te3, Ti3,}),
    ;

    public static final Pitch[][] DoMajWide = {
            {Do2, Re2, Mi2, Fa2, So2, La2, Ti2,},
            {Do3, Re3, Mi3, Fa3, So3, La3, Ti3,},
            {Do4, Re4, Mi4, Fa4, So4, La4, Ti4,},
            {Do5, Re5, Mi5, Fa5, So5, La5, Ti5,},
            {Do6, Re6, Mi6, Fa6, So6, La6, Ti6, Do7,},
    };
    public static final Pitch[][] RaMajWide = {
            {Do2, Ra2, Me2, Fa2, Fi2, Le2, Te2,},
            {Do3, Ra3, Me3, Fa3, Fi3, Le3, Te3,},
            {Do4, Ra4, Me4, Fa4, Fi4, Le4, Te4,},
            {Do5, Ra5, Me5, Fa5, Fi5, Le5, Te5,},
            {Do6, Ra6, Me6, Fa6, Fi6, Le6, Te6, Do7,},
    };
    public static final Pitch[][] ReMajWide = {
            {Ra2, Re2, Mi2, Fi2, So2, La2, Ti2,},
            {Ra3, Re3, Mi3, Fi3, So3, La3, Ti3,},
            {Ra4, Re4, Mi4, Fi4, So4, La4, Ti4,},
            {Ra5, Re5, Mi5, Fi5, So5, La5, Ti5,},
            {Ra6, Re6, Mi6, Fi6, So6, La6, Ti6,},
    };
    public static final Pitch[][] MeMajWide = {
            {Do2, Re2, Me2, Fa2, So2, Le2, Te2,},
            {Do3, Re3, Me3, Fa3, So3, Le3, Te3,},
            {Do4, Re4, Me4, Fa4, So4, Le4, Te4,},
            {Do5, Re5, Me5, Fa5, So5, Le5, Te5,},
            {Do6, Re6, Me6, Fa6, So6, Le6, Te6, Do7,},
    };
    public static final Pitch[][] MiMajWide = {
            {Ra2, Me2, Mi2, Fi2, Le2, La2, Ti2,},
            {Ra3, Me3, Mi3, Fi3, Le3, La3, Ti3,},
            {Ra4, Me4, Mi4, Fi4, Le4, La4, Ti4,},
            {Ra5, Me5, Mi5, Fi5, Le5, La5, Ti5,},
            {Ra6, Me6, Mi6, Fi6, Le6, La6, Ti6,},
    };
    public static final Pitch[][] FaMajWide = {
            {Do2, Re2, Mi2, Fa2, So2, La2, Te2,},
            {Do3, Re3, Mi3, Fa3, So3, La3, Te3,},
            {Do4, Re4, Mi4, Fa4, So4, La4, Te4,},
            {Do5, Re5, Mi5, Fa5, So5, La5, Te5,},
            {Do6, Re6, Mi6, Fa6, So6, La6, Te6, Do7,},
    };
    public static final Pitch[][] FiMajWide = {
            {Ra2, Me2, Fa2, Fi2, Le2, Te2, Ti2,},
            {Ra3, Me3, Fa3, Fi3, Le3, Te3, Ti3,},
            {Ra4, Me4, Fa4, Fi4, Le4, Te4, Ti4,},
            {Ra5, Me5, Fa5, Fi5, Le5, Te5, Ti5,},
            {Ra6, Me6, Fa6, Fi6, Le6, Te6, Ti6,},
    };
    public static final Pitch[][] SoMajWide = {
            {Do2, Re2, Mi2, Fi2, So2, La2, Ti2,},
            {Do3, Re3, Mi3, Fi3, So3, La3, Ti3,},
            {Do4, Re4, Mi4, Fi4, So4, La4, Ti4,},
            {Do5, Re5, Mi5, Fi5, So5, La5, Ti5,},
            {Do6, Re6, Mi6, Fi6, So6, La6, Ti6, Do7,},
    };
    public static final Pitch[][] LeMajWide = {
            {Do2, Ra2, Me2, Fa2, So2, Le2, Te2,},
            {Do3, Ra3, Me3, Fa3, So3, Le3, Te3,},
            {Do4, Ra4, Me4, Fa4, So4, Le4, Te4,},
            {Do5, Ra5, Me5, Fa5, So5, Le5, Te5,},
            {Do6, Ra6, Me6, Fa6, So6, Le6, Te6, Do7,},
    };
    public static final Pitch[][] LaMajWide = {
            {Ra2, Re2, Mi2, Fi2, Le2, La2, Ti2,},
            {Ra3, Re3, Mi3, Fi3, Le3, La3, Ti3,},
            {Ra4, Re4, Mi4, Fi4, Le4, La4, Ti4,},
            {Ra5, Re5, Mi5, Fi5, Le5, La5, Ti5,},
            {Ra6, Re6, Mi6, Fi6, Le6, La6, Ti6,},
    };
    public static final Pitch[][] TeMajWide = {
            {Do2, Re2, Me2, Fa2, So2, La2, Te2,},
            {Do3, Re3, Me3, Fa3, So3, La3, Te3,},
            {Do4, Re4, Me4, Fa4, So4, La4, Te4,},
            {Do5, Re5, Me5, Fa5, So5, La5, Te5,},
            {Do6, Re6, Me6, Fa6, So6, La6, Te6, Do7,},
    };
    public static final Pitch[][] TiMajWide = {
            {Ra2, Me2, Mi2, Fi2, Le2, Te2, Ti2,},
            {Ra3, Me3, Mi3, Fi3, Le3, Te3, Ti3,},
            {Ra4, Me4, Mi4, Fi4, Le4, Te4, Ti4,},
            {Ra5, Me5, Mi5, Fi5, Le5, Te5, Ti5,},
            {Ra6, Me6, Mi6, Fi6, Le6, Te6, Ti6,},
    };

    public static final Pitch[][] octavesDo2ToDo7 = {
            {Do2, Ra2, Re2, Me2, Mi2, Fa2, Fi2, So2, Le2, La2, Te2, Ti2,},
            {Do3, Ra3, Re3, Me3, Mi3, Fa3, Fi3, So3, Le3, La3, Te3, Ti3,},
            {Do4, Ra4, Re4, Me4, Mi4, Fa4, Fi4, So4, Le4, La4, Te4, Ti4,},
            {Do5, Ra5, Re5, Me5, Mi5, Fa5, Fi5, So5, Le5, La5, Te5, Ti5,},
            {Do6, Ra6, Re6, Me6, Mi6, Fa6, Fi6, So6, Le6, La6, Te6, Ti6, Do7,},
    };

    private final Pitch[] scale;

    Scale(Pitch[] scale) {
        this.scale = scale;
    }

    public Pitch[] getScale() {
        return scale;
    }


}