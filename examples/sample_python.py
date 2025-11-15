"""
Sample Python Code for VZCode Visualization
Demonstrates a data processing pipeline with multiple classes
"""

from abc import ABC, abstractmethod
from typing import List, Any
import json


class DataProcessor(ABC):
    """Abstract base class for data processors"""

    def __init__(self, name: str):
        self.name = name
        self.processed_count = 0

    @abstractmethod
    def process(self, data: Any) -> Any:
        """Process a single data item"""
        pass

    def process_batch(self, data_list: List[Any]) -> List[Any]:
        """Process a batch of data items"""
        results = []
        for item in data_list:
            result = self.process(item)
            if result is not None:
                results.append(result)
                self.processed_count += 1
        return results

    def get_stats(self) -> dict:
        """Get processor statistics"""
        return {
            'name': self.name,
            'processed': self.processed_count
        }


class Validator(DataProcessor):
    """Validates data against rules"""

    def __init__(self):
        super().__init__("Validator")
        self.rules = []

    def add_rule(self, rule: callable):
        """Add a validation rule"""
        self.rules.append(rule)

    def process(self, data: Any) -> Any:
        """Validate data against all rules"""
        for rule in self.rules:
            if not rule(data):
                return None
        return data


class Transformer(DataProcessor):
    """Transforms data using a transformation function"""

    def __init__(self, transform_func: callable):
        super().__init__("Transformer")
        self.transform_func = transform_func

    def process(self, data: Any) -> Any:
        """Transform the data"""
        try:
            return self.transform_func(data)
        except Exception as e:
            print(f"Transformation error: {e}")
            return None


class Aggregator(DataProcessor):
    """Aggregates data into summary statistics"""

    def __init__(self):
        super().__init__("Aggregator")
        self.values = []

    def process(self, data: Any) -> Any:
        """Add data to aggregation"""
        if isinstance(data, (int, float)):
            self.values.append(data)
        return data

    def get_summary(self) -> dict:
        """Get aggregation summary"""
        if not self.values:
            return {'count': 0}

        return {
            'count': len(self.values),
            'sum': sum(self.values),
            'avg': sum(self.values) / len(self.values),
            'min': min(self.values),
            'max': max(self.values)
        }


class Pipeline:
    """Data processing pipeline"""

    def __init__(self, name: str):
        self.name = name
        self.processors: List[DataProcessor] = []

    def add_processor(self, processor: DataProcessor) -> 'Pipeline':
        """Add a processor to the pipeline"""
        self.processors.append(processor)
        return self

    def execute(self, data: List[Any]) -> List[Any]:
        """Execute the pipeline on data"""
        current_data = data

        for processor in self.processors:
            print(f"Executing {processor.name}...")
            current_data = processor.process_batch(current_data)

            if not current_data:
                print(f"Pipeline stopped: no data after {processor.name}")
                break

        return current_data

    def get_report(self) -> dict:
        """Get pipeline execution report"""
        return {
            'pipeline': self.name,
            'processors': [p.get_stats() for p in self.processors],
            'total_processors': len(self.processors)
        }


def main():
    """Main execution function"""
    # Create pipeline
    pipeline = Pipeline("Data Processing Pipeline")

    # Add validator
    validator = Validator()
    validator.add_rule(lambda x: x is not None)
    validator.add_rule(lambda x: isinstance(x, (int, float)))
    validator.add_rule(lambda x: x >= 0)

    # Add transformer
    transformer = Transformer(lambda x: x * 2)

    # Add aggregator
    aggregator = Aggregator()

    # Build pipeline
    pipeline.add_processor(validator) \
            .add_processor(transformer) \
            .add_processor(aggregator)

    # Sample data
    data = [1, 2, -3, 4, None, 5, "invalid", 6, 7, 8]

    # Execute
    print("Starting pipeline execution...")
    results = pipeline.execute(data)

    # Print results
    print(f"\nResults: {results}")
    print(f"\nPipeline Report:")
    print(json.dumps(pipeline.get_report(), indent=2))
    print(f"\nAggregation Summary:")
    print(json.dumps(aggregator.get_summary(), indent=2))


if __name__ == "__main__":
    main()
