package com.pitchenga.domain;

public enum Hinter {
    Always("Hint: immediately", 0),
    Series("Hint: series", 0),
    Delayed100("Hint: after 100 ms", 100),
    Delayed200("Hint: after 200 ms", 200),
    Delayed300("Hint: after 300 ms", 300),
    Delayed400("Hint: after 400 ms", 400),
    Delayed500("Hint: after 500 ms", 500),
    Delayed600("Hint: after 600 ms", 600),
    Delayed700("Hint: after 700 ms", 700),
    Delayed800("Hint: after 800 ms", 800),
    Delayed900("Hint: after 900 ms", 900),
    Delayed1000("Hint: after 1 second", 1000),
    Delayed2000("Hint: after 2 seconds", 2000),
    Delayed3000("Hint: after 3 seconds", 3000),
    Never("Hint: never", Integer.MAX_VALUE);

    private final String name;
    public final int delayMs;

    Hinter(String name, int delayMs) {
        this.name = name;
        this.delayMs = delayMs;
    }

    @Override
    public String toString() {
        return name;
    }
}
