import java.awt.print.*;
import java.io.File;

class Main {
    private Main() {}

    private static void printHelp() {
        System.out.println("A Java Printer v0.1");
        System.out.println("USEAGE: java Main DIRECTORY");
        System.out.println("DIRECTORY: place full of files to print");
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            printHelp();
            return;
        }

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
        
        File[] files = new File(args[0]).listFiles();
        if (files == null) {
            System.err.println("Specified directory does not exist");
            return;
        }
        Printable printer = new MyPrinter(files);

        PrinterJob job = PrinterJob.getPrinterJob();
        job.setPrintable(printer, pf);

        System.out.print("Printing.");        

        try {
            job.print();
        } catch (PrinterException e) {
            System.err.println(e.getMessage());
        }
    }
}
