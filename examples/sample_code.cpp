// Sample C++ Code for VZCode Visualization
// This example demonstrates a simple data processing pipeline

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

// Abstract base class for data processors
class DataProcessor {
public:
    virtual ~DataProcessor() = default;
    virtual void process(std::vector<int>& data) = 0;
    virtual std::string getName() const = 0;
};

// Concrete processor: Doubles each value
class Doubler : public DataProcessor {
public:
    void process(std::vector<int>& data) override {
        for (auto& value : data) {
            value *= 2;
        }
    }

    std::string getName() const override {
        return "Doubler";
    }
};

// Concrete processor: Filters out odd numbers
class EvenFilter : public DataProcessor {
public:
    void process(std::vector<int>& data) override {
        data.erase(
            std::remove_if(data.begin(), data.end(),
                [](int n) { return n % 2 != 0; }),
            data.end()
        );
    }

    std::string getName() const override {
        return "EvenFilter";
    }
};

// Concrete processor: Sorts data
class Sorter : public DataProcessor {
public:
    void process(std::vector<int>& data) override {
        std::sort(data.begin(), data.end());
    }

    std::string getName() const override {
        return "Sorter";
    }
};

// Pipeline class that orchestrates multiple processors
class ProcessingPipeline {
private:
    std::vector<std::unique_ptr<DataProcessor>> processors_;
    std::vector<int> data_;

public:
    void addProcessor(std::unique_ptr<DataProcessor> processor) {
        processors_.push_back(std::move(processor));
    }

    void setData(const std::vector<int>& data) {
        data_ = data;
    }

    void execute() {
        std::cout << "Starting pipeline execution...\n";
        std::cout << "Initial data: ";
        printData();

        for (auto& processor : processors_) {
            std::cout << "\nApplying " << processor->getName() << "...\n";
            processor->process(data_);
            std::cout << "Result: ";
            printData();
        }

        std::cout << "\nPipeline execution complete!\n";
    }

    const std::vector<int>& getData() const {
        return data_;
    }

private:
    void printData() const {
        std::cout << "[";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i < data_.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }
};

// Main function demonstrating the pipeline
int main() {
    // Create pipeline
    ProcessingPipeline pipeline;

    // Add processors
    pipeline.addProcessor(std::make_unique<Doubler>());
    pipeline.addProcessor(std::make_unique<EvenFilter>());
    pipeline.addProcessor(std::make_unique<Sorter>());

    // Set initial data
    std::vector<int> initialData = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    pipeline.setData(initialData);

    // Execute pipeline
    pipeline.execute();

    // Display final results
    const auto& result = pipeline.getData();
    std::cout << "\nFinal processed data size: " << result.size() << "\n";

    return 0;
}
