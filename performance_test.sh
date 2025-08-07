#!/bin/bash
# Performance measurement script for particle simulation

echo "🚀 Particle Simulation Performance Test"
echo "======================================="
echo "Date: $(date)"
echo "System: $(uname -a)"
echo

# Build the project
echo "Building optimized version..."
./build.sh > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful"
echo

# Function to extract performance metrics from log
extract_metrics() {
    local logfile=$1
    echo "📊 Performance Metrics from $logfile:"
    
    # Extract FPS info
    if grep -q "FPS:" "$logfile"; then
        echo "  FPS: $(grep "FPS:" "$logfile" | tail -1 | cut -d' ' -f2)"
    fi
    
    # Extract timing info
    if grep -q "Performance Stats" "$logfile"; then
        echo "  Detailed timing available in log"
        grep -A 3 "Avg:" "$logfile" | head -6
    fi
    
    echo
}

# Test with current configuration
echo "🔍 Running baseline performance test..."
echo "  Particles: 6000 (default)"
echo "  Duration: 10 seconds"

timeout 10s ./build/particle_sim > performance_baseline.log 2>&1

if [ -f performance_baseline.log ]; then
    extract_metrics performance_baseline.log
else
    echo "❌ Test failed - no log generated"
fi

# Quick memory usage check
echo "💾 Memory Usage Analysis:"
if command -v ps &> /dev/null; then
    echo "  Process memory usage available via: ps -o pid,vsz,rss,comm -p \$PID"
fi

if command -v perf &> /dev/null; then
    echo "  Advanced profiling available via: perf stat ./build/particle_sim"
else
    echo "  Install perf tools for detailed analysis: sudo apt install linux-tools-generic"
fi

echo
echo "📝 Performance Tips:"
echo "  1. Monitor the detailed stats that print every 5 seconds"
echo "  2. Look for frame drops in the FPS counter"
echo "  3. Check which phase takes the most time (collision vs rendering)"
echo "  4. Use 'htop' to monitor CPU usage while running"
echo
echo "🎯 Good Performance Targets:"
echo "  - Collision Detection: <2ms per frame"
echo "  - Rendering: <5ms per frame"
echo "  - Stable 60 FPS with 6000 particles"
echo
echo "🏁 Test complete! Run './build/particle_sim' to see live performance stats."
