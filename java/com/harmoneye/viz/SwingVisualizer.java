package com.harmoneye.viz;

import java.awt.*;

public interface SwingVisualizer<T> extends Visualizer<T> {

    Component getComponent();

}