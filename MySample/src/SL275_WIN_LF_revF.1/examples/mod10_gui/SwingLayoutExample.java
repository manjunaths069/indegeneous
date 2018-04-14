import java.awt.*;
import javax.swing.*;

public class SwingLayoutExample {
  private JFrame f;
  private JButton b1;
  private JButton b2;

  public SwingLayoutExample() {
    f = new JFrame("GUI example");
    b1 = new JButton("Press Me");
    b2 = new JButton("Don't press Me");
  }

  public void launchFrame() {
    f.getContentPane().setLayout(new FlowLayout());
    f.getContentPane().add(b1);
    f.getContentPane().add(b2);
    f.pack();
    f.setVisible(true);
  }

  public static void main(String args[]) {
    SwingLayoutExample guiWindow = new SwingLayoutExample();
    guiWindow.launchFrame();
  }
}
