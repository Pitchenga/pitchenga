package com.pitchenga.domain;

import com.pitchenga.Player;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.LinkedList;
import java.util.List;
import java.util.zip.ZipInputStream;

import static com.pitchenga.domain.Tone.*;

public enum Pitch {
    Non(Do, -1, -1, -1f),
    Do0(Do, 0, 12, 16.35f),
    Ra0(Ra, 0, 13, 17.32f),
    Re0(Re, 0, 14, 18.35f),
    Me0(Me, 0, 15, 19.45f),
    Mi0(Mi, 0, 16, 20.6f),
    Fa0(Fa, 0, 17, 21.83f),
    Fi0(Fi, 0, 18, 23.12f),
    So0(So, 0, 19, 24.5f),
    Le0(Le, 0, 20, 25.96f),
    La0(La, 0, 21, 27.5f),
    Te0(Te, 0, 22, 29.14f),
    Ti0(Ti, 0, 23, 30.87f),
    Do1(Do, 1, 24, 32.7f),
    Ra1(Ra, 1, 25, 34.65f),
    Re1(Re, 1, 26, 36.71f),
    Me1(Me, 1, 27, 38.89f),
    Mi1(Mi, 1, 28, 41.2f),
    Fa1(Fa, 1, 29, 43.65f),
    Fi1(Fi, 1, 30, 46.25f),
    So1(So, 1, 31, 49f),
    Le1(Le, 1, 32, 51.91f),
    La1(La, 1, 33, 55f),
    Te1(Te, 1, 34, 58.27f),
    Ti1(Ti, 1, 35, 61.74f),
    Do2(Do, 2, 36, 65.41f),
    Ra2(Ra, 2, 37, 69.3f),
    Re2(Re, 2, 38, 73.42f),
    Me2(Me, 2, 39, 77.78f),
    Mi2(Mi, 2, 40, 82.41f),
    Fa2(Fa, 2, 41, 87.31f),
    Fi2(Fi, 2, 42, 92.5f),
    So2(So, 2, 43, 98f),
    Le2(Le, 2, 44, 103.83f),
    La2(La, 2, 45, 110f),
    Te2(Te, 2, 46, 116.54f),
    Ti2(Ti, 2, 47, 123.47f),
    Do3(Do, 3, 48, 130.81f),
    Ra3(Ra, 3, 49, 138.59f),
    Re3(Re, 3, 50, 146.83f),
    Me3(Me, 3, 51, 155.56f),
    Mi3(Mi, 3, 52, 164.81f),
    Fa3(Fa, 3, 53, 174.61f),
    Fi3(Fi, 3, 54, 185f),
    So3(So, 3, 55, 196f),
    Le3(Le, 3, 56, 207.65f),
    La3(La, 3, 57, 220f),
    Te3(Te, 3, 58, 233.08f),
    Ti3(Ti, 3, 59, 246.94f),
    Do4(Do, 4, 60, 261.63f),
    Ra4(Ra, 4, 61, 277.18f),
    Re4(Re, 4, 62, 293.66f),
    Me4(Me, 4, 63, 311.13f),
    Mi4(Mi, 4, 64, 329.63f),
    Fa4(Fa, 4, 65, 349.23f),
    Fi4(Fi, 4, 66, 369.99f),
    So4(So, 4, 67, 392f),
    Le4(Le, 4, 68, 415.3f),
    La4(La, 4, 69, 440f),
    Te4(Te, 4, 70, 466.16f),
    Ti4(Ti, 4, 71, 493.88f),
    Do5(Do, 5, 72, 523.25f),
    Ra5(Ra, 5, 73, 554.37f),
    Re5(Re, 5, 74, 587.33f),
    Me5(Me, 5, 75, 622.25f),
    Mi5(Mi, 5, 76, 659.25f),
    Fa5(Fa, 5, 77, 698.46f),
    Fi5(Fi, 5, 78, 739.99f),
    So5(So, 5, 79, 783.99f),
    Le5(Le, 5, 80, 830.61f),
    La5(La, 5, 81, 880f),
    Te5(Te, 5, 82, 932.33f),
    Ti5(Ti, 5, 83, 987.77f),
    Do6(Do, 6, 84, 1046.5f),
    Ra6(Ra, 6, 85, 1108.73f),
    Re6(Re, 6, 86, 1174.66f),
    Me6(Me, 6, 87, 1244.51f),
    Mi6(Mi, 6, 88, 1318.51f),
    Fa6(Fa, 6, 89, 1396.91f),
    Fi6(Fi, 6, 90, 1479.98f),
    So6(So, 6, 91, 1567.98f),
    Le6(Le, 6, 92, 1661.22f),
    La6(La, 6, 93, 1760f),
    Te6(Te, 6, 94, 1864.66f),
    Ti6(Ti, 6, 95, 1975.53f),
    Do7(Do, 7, 96, 2093f),
    Ra7(Ra, 7, 97, 2217.46f),
    Re7(Re, 7, 98, 2349.32f),
    Me7(Me, 7, 99, 2489.02f),
    Mi7(Mi, 7, 100, 2637.02f),
    Fa7(Fa, 7, 101, 2793.83f),
    Fi7(Fi, 7, 102, 2959.96f),
    So7(So, 7, 103, 3135.96f),
    Le7(Le, 7, 104, 3322.44f),
    La7(La, 7, 105, 3520f),
    Te7(Te, 7, 106, 3729.31f),
    Ti7(Ti, 7, 107, 3951.07f),
    Do8(Do, 8, 108, 4186.01f),
    Ra8(Ra, 8, 109, 4434.92f),
    Re8(Re, 8, 110, 4698.63f),
    Me8(Me, 8, 111, 4978.03f),
    Mi8(Mi, 8, 112, 5274.04f),
    Fa8(Fa, 8, 113, 5587.65f),
    Fi8(Fi, 8, 114, 5919.91f),
    So8(So, 8, 115, 6271.93f),
    Le8(Le, 8, 116, 6644.88f),
    La8(La, 8, 117, 7040f),
    Te8(Te, 8, 118, 7458.62f),
    Ti8(Ti, 8, 119, 7902.13f),
    ;

    public final Tone tone;
    public final int midi;
    public final float frequency;
    public final int octave;
    public final String note;
    public final String name;
    public final Player player;
    public final int number;
    public final double[][] video;
    private volatile Fugue fugue;

    Pitch(Tone tone, int octave, int midi, float frequency) {
        this.tone = tone;
        this.octave = octave;
        this.midi = midi;
        this.frequency = frequency;
        this.note = tone.west + octave;
        this.name = name().toLowerCase();
        this.number = octave * 100 + tone.ordinal();
        URL wavUrl = getClass().getResource("/wav/" + midi + ".wav");
        if (wavUrl == null) {
            player = null;
        } else {
            player = new Player(wavUrl);
        }

        String videoZipResourcePath = String.format("/video/%03d.zip", number);
        URL videoZipUrl = getClass().getResource(videoZipResourcePath);
        double[][] video = null;
        if (videoZipUrl == null) {
            System.out.println("Resource not found=" + videoZipResourcePath);
        } else {
            try {
                System.out.println("Reading resource=" + videoZipResourcePath);
                List<double[]> frames = new LinkedList<>();
                ZipInputStream zipInputStream = new ZipInputStream(videoZipUrl.openStream());
                zipInputStream.getNextEntry();
                BufferedReader reader = new BufferedReader(new InputStreamReader(zipInputStream));
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] strings = line.split(" ");
                    double[] frame = new double[strings.length];
                    for (int i = 0; i < strings.length; i++) {
                        double velocity = Double.parseDouble(strings[i]);
                        if (velocity < 0.81) {
                            velocity *= 0.6;
                        }
                        frame[i] = velocity;
                    }
                    frames.add(frame);
                }
                video = frames.toArray(new double[0][]);
            } catch (IOException e) {
                System.out.println("Failed reading resource=" + videoZipResourcePath + ": " + e);
                e.printStackTrace();
            }
        }
        this.video = video;
    }

    public Fugue getFugue() {
        if (this.fugue == null) {
            for (Fugue aFugue : Fugue.values()) {
                if (aFugue.pitch.equals(this)) {
                    this.fugue = aFugue;
                    return aFugue;
                }
            }
            for (Fugue aFugue : Fugue.values()) {
                if (aFugue.pitch.tone.equals(this.tone)) {
                    this.fugue = aFugue;
                    return aFugue;
                }
            }
            throw new IllegalArgumentException("Fugue not found for=" + this);
        } else {
            return this.fugue;
        }
    }

}