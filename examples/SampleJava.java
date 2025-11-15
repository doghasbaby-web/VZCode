/**
 * Sample Java Code for VZCode Visualization
 * Demonstrates a library management system with multiple classes and inheritance
 */

import java.util.*;
import java.time.LocalDate;
import java.time.temporal.ChronoUnit;

// Abstract base class for library items
abstract class LibraryItem {
    protected String id;
    protected String title;
    protected boolean isAvailable;
    protected LocalDate dueDate;

    public LibraryItem(String id, String title) {
        this.id = id;
        this.title = title;
        this.isAvailable = true;
        this.dueDate = null;
    }

    public abstract String getItemType();
    public abstract double calculateLateFee(LocalDate returnDate);

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public boolean isAvailable() {
        return isAvailable;
    }

    public void setAvailable(boolean available) {
        this.isAvailable = available;
    }

    public LocalDate getDueDate() {
        return dueDate;
    }

    public void setDueDate(LocalDate dueDate) {
        this.dueDate = dueDate;
    }
}

// Book class
class Book extends LibraryItem {
    private String author;
    private String isbn;
    private int pageCount;

    public Book(String id, String title, String author, String isbn, int pageCount) {
        super(id, title);
        this.author = author;
        this.isbn = isbn;
        this.pageCount = pageCount;
    }

    @Override
    public String getItemType() {
        return "Book";
    }

    @Override
    public double calculateLateFee(LocalDate returnDate) {
        if (dueDate == null || !returnDate.isAfter(dueDate)) {
            return 0.0;
        }
        long daysLate = ChronoUnit.DAYS.between(dueDate, returnDate);
        return daysLate * 0.50; // $0.50 per day
    }

    public String getAuthor() {
        return author;
    }

    public String getIsbn() {
        return isbn;
    }
}

// Magazine class
class Magazine extends LibraryItem {
    private String publisher;
    private int issueNumber;
    private LocalDate publishDate;

    public Magazine(String id, String title, String publisher, int issueNumber, LocalDate publishDate) {
        super(id, title);
        this.publisher = publisher;
        this.issueNumber = issueNumber;
        this.publishDate = publishDate;
    }

    @Override
    public String getItemType() {
        return "Magazine";
    }

    @Override
    public double calculateLateFee(LocalDate returnDate) {
        if (dueDate == null || !returnDate.isAfter(dueDate)) {
            return 0.0;
        }
        long daysLate = ChronoUnit.DAYS.between(dueDate, returnDate);
        return daysLate * 0.25; // $0.25 per day
    }
}

// DVD class
class DVD extends LibraryItem {
    private String director;
    private int durationMinutes;
    private String genre;

    public DVD(String id, String title, String director, int durationMinutes, String genre) {
        super(id, title);
        this.director = director;
        this.durationMinutes = durationMinutes;
        this.genre = genre;
    }

    @Override
    public String getItemType() {
        return "DVD";
    }

    @Override
    public double calculateLateFee(LocalDate returnDate) {
        if (dueDate == null || !returnDate.isAfter(dueDate)) {
            return 0.0;
        }
        long daysLate = ChronoUnit.DAYS.between(dueDate, returnDate);
        return daysLate * 1.00; // $1.00 per day
    }
}

// Library Member class
class LibraryMember {
    private String memberId;
    private String name;
    private String email;
    private List<LibraryItem> borrowedItems;
    private LocalDate membershipDate;

    public LibraryMember(String memberId, String name, String email) {
        this.memberId = memberId;
        this.name = name;
        this.email = email;
        this.borrowedItems = new ArrayList<>();
        this.membershipDate = LocalDate.now();
    }

    public boolean borrowItem(LibraryItem item) {
        if (item.isAvailable() && borrowedItems.size() < 5) {
            borrowedItems.add(item);
            item.setAvailable(false);
            item.setDueDate(LocalDate.now().plusDays(14)); // 2 weeks
            return true;
        }
        return false;
    }

    public boolean returnItem(LibraryItem item) {
        if (borrowedItems.remove(item)) {
            item.setAvailable(true);
            item.setDueDate(null);
            return true;
        }
        return false;
    }

    public List<LibraryItem> getBorrowedItems() {
        return new ArrayList<>(borrowedItems);
    }

    public String getMemberId() {
        return memberId;
    }

    public String getName() {
        return name;
    }

    public int getBorrowedCount() {
        return borrowedItems.size();
    }
}

// Library class
class Library {
    private String name;
    private Map<String, LibraryItem> catalog;
    private Map<String, LibraryMember> members;

    public Library(String name) {
        this.name = name;
        this.catalog = new HashMap<>();
        this.members = new HashMap<>();
    }

    public void addItem(LibraryItem item) {
        catalog.put(item.getId(), item);
    }

    public void registerMember(LibraryMember member) {
        members.put(member.getMemberId(), member);
    }

    public LibraryItem searchById(String id) {
        return catalog.get(id);
    }

    public List<LibraryItem> searchByTitle(String title) {
        List<LibraryItem> results = new ArrayList<>();
        for (LibraryItem item : catalog.values()) {
            if (item.getTitle().toLowerCase().contains(title.toLowerCase())) {
                results.add(item);
            }
        }
        return results;
    }

    public boolean checkoutItem(String memberId, String itemId) {
        LibraryMember member = members.get(memberId);
        LibraryItem item = catalog.get(itemId);

        if (member != null && item != null) {
            return member.borrowItem(item);
        }
        return false;
    }

    public boolean returnItem(String memberId, String itemId) {
        LibraryMember member = members.get(memberId);
        LibraryItem item = catalog.get(itemId);

        if (member != null && item != null) {
            return member.returnItem(item);
        }
        return false;
    }

    public double calculateFees(String memberId, LocalDate returnDate) {
        LibraryMember member = members.get(memberId);
        if (member == null) {
            return 0.0;
        }

        double totalFees = 0.0;
        for (LibraryItem item : member.getBorrowedItems()) {
            totalFees += item.calculateLateFee(returnDate);
        }
        return totalFees;
    }

    public Map<String, Integer> getStatistics() {
        Map<String, Integer> stats = new HashMap<>();
        stats.put("totalItems", catalog.size());
        stats.put("totalMembers", members.size());

        int availableItems = 0;
        int borrowedItems = 0;

        for (LibraryItem item : catalog.values()) {
            if (item.isAvailable()) {
                availableItems++;
            } else {
                borrowedItems++;
            }
        }

        stats.put("availableItems", availableItems);
        stats.put("borrowedItems", borrowedItems);

        return stats;
    }

    public void printCatalog() {
        System.out.println("\n=== " + name + " Catalog ===");
        for (LibraryItem item : catalog.values()) {
            System.out.printf("%s - %s (%s) - %s\n",
                item.getId(),
                item.getTitle(),
                item.getItemType(),
                item.isAvailable() ? "Available" : "Borrowed"
            );
        }
    }
}

// Main demo class
public class SampleJava {
    public static void main(String[] args) {
        System.out.println("=== Library Management System Demo ===\n");

        // Create library
        Library library = new Library("City Central Library");

        // Add books
        library.addItem(new Book("B001", "The Great Gatsby", "F. Scott Fitzgerald", "978-0743273565", 180));
        library.addItem(new Book("B002", "1984", "George Orwell", "978-0451524935", 328));
        library.addItem(new Book("B003", "To Kill a Mockingbird", "Harper Lee", "978-0060935467", 324));

        // Add magazines
        library.addItem(new Magazine("M001", "National Geographic", "National Geographic Society", 145, LocalDate.of(2024, 1, 1)));
        library.addItem(new Magazine("M002", "TIME", "Time Inc.", 52, LocalDate.of(2024, 1, 8)));

        // Add DVDs
        library.addItem(new DVD("D001", "The Shawshank Redemption", "Frank Darabont", 142, "Drama"));
        library.addItem(new DVD("D002", "Inception", "Christopher Nolan", 148, "Sci-Fi"));

        // Register members
        LibraryMember john = new LibraryMember("M001", "John Doe", "john@example.com");
        LibraryMember jane = new LibraryMember("M002", "Jane Smith", "jane@example.com");

        library.registerMember(john);
        library.registerMember(jane);

        // Display catalog
        library.printCatalog();

        // Checkout items
        System.out.println("\n=== Checkouts ===");
        System.out.println("John borrows '1984': " + library.checkoutItem("M001", "B002"));
        System.out.println("John borrows 'Inception': " + library.checkoutItem("M001", "D002"));
        System.out.println("Jane borrows 'The Great Gatsby': " + library.checkoutItem("M002", "B001"));

        // Display updated catalog
        library.printCatalog();

        // Display statistics
        System.out.println("\n=== Library Statistics ===");
        Map<String, Integer> stats = library.getStatistics();
        for (Map.Entry<String, Integer> entry : stats.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }

        // Calculate late fees (simulating items are 5 days late)
        System.out.println("\n=== Late Fees (5 days late) ===");
        LocalDate returnDate = LocalDate.now().plusDays(19); // 19 days after checkout = 5 days late
        double johnFees = library.calculateFees("M001", returnDate);
        double janeFees = library.calculateFees("M002", returnDate);

        System.out.printf("John's late fees: $%.2f\n", johnFees);
        System.out.printf("Jane's late fees: $%.2f\n", janeFees);

        // Search functionality
        System.out.println("\n=== Search Results for '1984' ===");
        List<LibraryItem> searchResults = library.searchByTitle("1984");
        for (LibraryItem item : searchResults) {
            System.out.println(item.getTitle() + " - " + item.getItemType());
        }
    }
}
