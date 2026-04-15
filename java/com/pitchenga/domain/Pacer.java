package com.pitchenga.domain;

public enum Pacer {
    Answer("Answer to continue", 0),
    Tempo20("Tempo 20", 20),
    Tempo25("Tempo 25", 25),
    Tempo30("Tempo 30", 30),
    Tempo35("Tempo 35", 35),
    Tempo40("Tempo 40", 40),
    Tempo45("Tempo 45", 45),
    Tempo50("Tempo 50", 50),
    Tempo55("Tempo 55", 55),
    Tempo60("Tempo 60", 60),
    Tempo65("Tempo 65", 65),
    Tempo70("Tempo 70", 70),
    Tempo75("Tempo 75", 75),
    Tempo80("Tempo 80", 80),
    Tempo85("Tempo 85", 85),
    Tempo90("Tempo 90", 90),
    Tempo95("Tempo 95", 95),
    Tempo100("Tempo 100", 100),
    Tempo105("Tempo 105", 105),
    Tempo110("Tempo 110", 110),
    Tempo115("Tempo 115", 115),
    Tempo120("Tempo 120", 120),
    Tempo125("Tempo 125", 125),
    Tempo130("Tempo 130", 130),
    Tempo135("Tempo 135", 135),
    Tempo140("Tempo 140", 140),
    Tempo145("Tempo 145", 145),
    Tempo150("Tempo 150", 150),
    Tempo155("Tempo 155", 155),
    Tempo160("Tempo 160", 160),
    Tempo165("Tempo 165", 165),
    Tempo170("Tempo 170", 170),
    Tempo175("Tempo 175", 175),
    Tempo180("Tempo 180", 180),
    Tempo190("Tempo 190", 190),
    Tempo200("Tempo 200", 200),
    Tempo210("Tempo 210", 210),
    Tempo220("Tempo 220", 220),
    Tempo230("Tempo 230", 230),
    Tempo240("Tempo 240", 240),
    Tempo250("Tempo 250", 250),
    Tempo260("Tempo 260", 260),
    Tempo270("Tempo 270", 270),
    Tempo280("Tempo 280", 280),
    Tempo290("Tempo 290", 290),
    Tempo300("Tempo 300", 300),
    ;

    private final String name;
    public final int bpm;

    Pacer(String name, int bpm) {
        this.name = name;
        this.bpm = bpm;
    }

    @Override
    public String toString() {
        return name;
    }
}
