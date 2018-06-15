import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.print.*;
import java.io.*;

import javax.imageio.IIOException;

class MyPrinter implements Printable {

    private static int LINE_HEIGHT = 12;

    @Override
    public int print(Graphics g,
        PageFormat pf, int pageIndex) {

            if (pageIndex > 0) {
                return Printable.NO_SUCH_PAGE;
            }

            Graphics2D g2d  = (Graphics2D) g;

            g2d.translate(pf.getImageableX(), pf.getImageableY());

            String fileName = "C:\\Cloud\\OneDrive - The Ohio State University\\CSE 2231\\submissions\\chaisherwin\\src\\Set3a.java";
            BufferedReader reader = null;
            try {
                reader = new BufferedReader(new FileReader(fileName));
                String line = reader.readLine();
                int lineCount = MyPrinter.LINE_HEIGHT;
                while (line != null) {
                    g.drawString(line, 0, lineCount);
                    line = reader.readLine();
                    lineCount += MyPrinter.LINE_HEIGHT;
                }
                reader.close();
            } catch (IOException e) {
                System.err.println(e.getMessage());
                if (reader != null) {
                    try {
                        reader.close();
                    } catch (IOException e2) {
                        System.err.println(e2.getMessage());
                    }
                }
            }

            return Printable.PAGE_EXISTS;
        }
}