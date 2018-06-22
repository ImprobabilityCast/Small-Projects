import java.awt.print.*;

class Main {
    private Main() {}

    public static void main(String[] args) {
        Paper paper = new Paper();
        // Units are 1/72 inch
        final int marginSpace = 12;
        double newImgWidth = paper.getWidth() - marginSpace * 2;
        double newImgHeight = paper.getHeight() - marginSpace * 2;
        paper.setImageableArea(marginSpace, marginSpace,
                newImgWidth, newImgHeight);

        PageFormat pf = new PageFormat();
        pf.setOrientation(PageFormat.LANDSCAPE);
        pf.setPaper(paper);

        Printable printer = new MyPrinter("C:\\Cloud\\OneDrive - The Ohio State University\\CSE 2231\\submissions\\chaisherwin\\src\\Set3a.java");

        PrinterJob job = PrinterJob.getPrinterJob();
        job.setPrintable(printer, pf);

        try {
            job.print();
        } catch (PrinterException e) {
            System.err.println(e.getMessage());
        }
    }
}
