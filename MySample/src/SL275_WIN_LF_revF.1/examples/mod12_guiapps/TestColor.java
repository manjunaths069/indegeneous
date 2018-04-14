import java.awt.*;
public class TestColor {
  public static void main(String[] args) {
    TestColor tester = new TestColor();
    tester.launchFrame();
  }
  TestColor() {}
  private void launchFrame() {
    Frame f = new Frame();
    Button b = new Button("Purple");
    Color purple = new Color(255, 0, 255);
    b.setBackground(purple);
    f.add(b);
    f.pack();
    f.setVisible(true);
  }
}
