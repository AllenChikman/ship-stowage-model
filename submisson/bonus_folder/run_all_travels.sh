#!/bin/bash
./simulator -travel_path ./travel_fatal -output ./results/output_fatal
./simulator -travel_path ./travel_mixed_warnings -output ./results/output_mixed
./simulator -travel_path ./travel_specific_warnings -output ./results/output_specific
./simulator -travel_path ./travel_valid -output ./results/output_valid
