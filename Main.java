import java.awt.print.*;


class Main {
    private Main() {}

    public static void main(String[] args) {
        PrinterJob job = PrinterJob.getPrinterJob();
        job.setPrintable(new MyPrinter());
        try {
        job.print();
        } catch (PrinterException e) {
            System.err.println(e.getMessage());
        }
    }
}
