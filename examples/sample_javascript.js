/**
 * Sample JavaScript Code for VZCode Visualization
 * Demonstrates an e-commerce shopping cart system
 */

// Product class
class Product {
    constructor(id, name, price, category) {
        this.id = id;
        this.name = name;
        this.price = price;
        this.category = category;
        this.inStock = true;
    }

    getDetails() {
        return {
            id: this.id,
            name: this.name,
            price: this.price,
            category: this.category,
            inStock: this.inStock
        };
    }

    setStock(inStock) {
        this.inStock = inStock;
    }

    applyDiscount(percentage) {
        if (percentage > 0 && percentage <= 100) {
            this.price = this.price * (1 - percentage / 100);
        }
    }
}

// Cart Item class
class CartItem {
    constructor(product, quantity = 1) {
        this.product = product;
        this.quantity = quantity;
    }

    getTotal() {
        return this.product.price * this.quantity;
    }

    updateQuantity(quantity) {
        if (quantity > 0) {
            this.quantity = quantity;
            return true;
        }
        return false;
    }
}

// Shopping Cart class
class ShoppingCart {
    constructor(userId) {
        this.userId = userId;
        this.items = [];
        this.discountCode = null;
    }

    addItem(product, quantity = 1) {
        // Check if product already in cart
        const existingItem = this.items.find(
            item => item.product.id === product.id
        );

        if (existingItem) {
            existingItem.quantity += quantity;
        } else {
            this.items.push(new CartItem(product, quantity));
        }

        return this.items.length;
    }

    removeItem(productId) {
        const index = this.items.findIndex(
            item => item.product.id === productId
        );

        if (index !== -1) {
            this.items.splice(index, 1);
            return true;
        }
        return false;
    }

    updateItemQuantity(productId, quantity) {
        const item = this.items.find(
            item => item.product.id === productId
        );

        if (item) {
            return item.updateQuantity(quantity);
        }
        return false;
    }

    getSubtotal() {
        return this.items.reduce((total, item) => {
            return total + item.getTotal();
        }, 0);
    }

    applyDiscount(code) {
        this.discountCode = code;
    }

    getDiscount() {
        if (!this.discountCode) return 0;

        const discounts = {
            'SAVE10': 0.10,
            'SAVE20': 0.20,
            'SAVE30': 0.30
        };

        return discounts[this.discountCode] || 0;
    }

    getTotal() {
        const subtotal = this.getSubtotal();
        const discount = this.getDiscount();
        return subtotal * (1 - discount);
    }

    isEmpty() {
        return this.items.length === 0;
    }

    clear() {
        this.items = [];
        this.discountCode = null;
    }

    getSummary() {
        return {
            userId: this.userId,
            itemCount: this.items.length,
            subtotal: this.getSubtotal(),
            discount: this.getDiscount(),
            total: this.getTotal(),
            items: this.items.map(item => ({
                name: item.product.name,
                quantity: item.quantity,
                price: item.product.price,
                total: item.getTotal()
            }))
        };
    }
}

// Order class
class Order {
    constructor(cart, shippingAddress) {
        this.orderId = this.generateOrderId();
        this.userId = cart.userId;
        this.items = [...cart.items];
        this.total = cart.getTotal();
        this.shippingAddress = shippingAddress;
        this.status = 'pending';
        this.createdAt = new Date();
    }

    generateOrderId() {
        return 'ORD-' + Date.now() + '-' + Math.random().toString(36).substr(2, 9);
    }

    updateStatus(newStatus) {
        const validStatuses = ['pending', 'processing', 'shipped', 'delivered', 'cancelled'];

        if (validStatuses.includes(newStatus)) {
            this.status = newStatus;
            return true;
        }
        return false;
    }

    getOrderDetails() {
        return {
            orderId: this.orderId,
            userId: this.userId,
            itemCount: this.items.length,
            total: this.total,
            status: this.status,
            shippingAddress: this.shippingAddress,
            createdAt: this.createdAt
        };
    }
}

// Payment Processor class
class PaymentProcessor {
    constructor() {
        this.transactions = [];
    }

    processPayment(order, paymentMethod) {
        // Simulate payment processing
        const transaction = {
            transactionId: this.generateTransactionId(),
            orderId: order.orderId,
            amount: order.total,
            paymentMethod: paymentMethod,
            status: 'success',
            timestamp: new Date()
        };

        this.transactions.push(transaction);
        return transaction;
    }

    generateTransactionId() {
        return 'TXN-' + Date.now() + '-' + Math.random().toString(36).substr(2, 9);
    }

    refund(transactionId) {
        const transaction = this.transactions.find(
            t => t.transactionId === transactionId
        );

        if (transaction && transaction.status === 'success') {
            transaction.status = 'refunded';
            return true;
        }
        return false;
    }

    getTransactionHistory(orderId) {
        return this.transactions.filter(t => t.orderId === orderId);
    }
}

// Demo usage
function runDemo() {
    console.log('=== E-Commerce Shopping Cart Demo ===\n');

    // Create products
    const laptop = new Product(1, 'Laptop', 999.99, 'Electronics');
    const mouse = new Product(2, 'Wireless Mouse', 29.99, 'Electronics');
    const keyboard = new Product(3, 'Mechanical Keyboard', 79.99, 'Electronics');

    // Create shopping cart
    const cart = new ShoppingCart('user123');

    // Add items
    cart.addItem(laptop, 1);
    cart.addItem(mouse, 2);
    cart.addItem(keyboard, 1);

    console.log('Cart Summary:');
    console.log(JSON.stringify(cart.getSummary(), null, 2));

    // Apply discount
    cart.applyDiscount('SAVE10');

    console.log('\nAfter applying SAVE10 discount:');
    console.log('Total: $' + cart.getTotal().toFixed(2));

    // Create order
    const shippingAddress = {
        street: '123 Main St',
        city: 'Springfield',
        state: 'IL',
        zip: '62701'
    };

    const order = new Order(cart, shippingAddress);

    console.log('\nOrder Created:');
    console.log(JSON.stringify(order.getOrderDetails(), null, 2));

    // Process payment
    const paymentProcessor = new PaymentProcessor();
    const transaction = paymentProcessor.processPayment(order, 'Credit Card');

    console.log('\nPayment Processed:');
    console.log(JSON.stringify(transaction, null, 2));

    // Update order status
    order.updateStatus('processing');
    order.updateStatus('shipped');

    console.log('\nFinal Order Status:', order.status);
}

// Run demo
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        Product,
        CartItem,
        ShoppingCart,
        Order,
        PaymentProcessor
    };
} else {
    runDemo();
}
