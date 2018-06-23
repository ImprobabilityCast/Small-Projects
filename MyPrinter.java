import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.print.Printable;
import java.awt.print.PageFormat;
import java.io.*;
import java.util.ArrayDeque;
import java.util.List;
import java.util.LinkedList;
import java.util.Queue;


class MyPrinter implements Printable {

    private static int LINE_HEIGHT = 10;

    private static float FONT_SIZE = 10.0f;

    /**
     * Index of the last page in the current file.
     */
    private int lastPage;

    /**
     * Keep track of a page during mutiple calls to print
     * with the same index.
     */
    private int previousIndex;

    private int position;

    /**
     * Holds all the lines of the current file.
     */
    private List<String> lines;

    /**
     * List of files to print.
     */
    private Queue<String> fileNames;

    /*
     * Constructors
     */

    public MyPrinter(String... files) {
        this.lastPage = -1;
        this.previousIndex = -1;
        this.lines = new LinkedList<>();
        this.fileNames = new ArrayDeque<>();
        for (String f : files) {
            this.fileNames.add(f);
        }
    }

    /**
     * Reads the file specified by fileName into buffer.
     * replaces buffer.
     */
    private static void fillBuffer(List<String> buffer, String fileName) {
        buffer.clear();
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(fileName));
            String line = reader.readLine();
            while (line != null) {
                buffer.add(line);
                line = reader.readLine();
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
    }

    /**
     * ensures that the buffer.size is a constant multiple of pageSize
     * and that buffer.size is > 0
     */
    private static void padBuffer(List<String> buffer, int pageSize) {
        if (buffer.size() == 0) {
            buffer.add("");
        }
        while (buffer.size() % pageSize != 0) {
            buffer.add("");
        }
    }

    @Override
    public int print(Graphics g, PageFormat pf, int pageIndex) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.translate(pf.getImageableX(), pf.getImageableY());
        g2d.setFont(g2d.getFont().deriveFont(MyPrinter.FONT_SIZE));

        final int linesPerPage = ((int) pf.getImageableHeight() / MyPrinter.LINE_HEIGHT) * 2; 

        if (pageIndex == this.previousIndex) {
            this.position -= linesPerPage;
        } else {
            this.previousIndex = pageIndex;

            if (pageIndex > this.lastPage) {
                if (this.fileNames.size() > 0) {
                    MyPrinter.fillBuffer(this.lines, this.fileNames.poll());
                    MyPrinter.padBuffer(this.lines, linesPerPage);
                    this.lastPage += this.lines.size() / linesPerPage;
                } else {
                    return Printable.NO_SUCH_PAGE;
                }
            }
        }

        int rightPos = this.position + (linesPerPage / 2);
        int yCoord = MyPrinter.LINE_HEIGHT;
        final int farX = (int) pf.getImageableWidth() / 2;
        
        while (yCoord < pf.getImageableHeight()) {
            g2d.drawString(this.lines.get(this.position), 0, yCoord);
            g2d.drawString(this.lines.get(rightPos), farX, yCoord);
            
            yCoord += MyPrinter.LINE_HEIGHT;
            this.position++;
            rightPos++;
        }

        this.position += linesPerPage / 2;

        return Printable.PAGE_EXISTS;
    }
}
