#!/usr/bin/env python3
"""
Verify Conditional Compilation System

This script validates that the BACnet for Arduino library correctly implements
conditional compilation based on board tier and feature availability.

Checks:
1. All object wrapper files have proper #if BACNET_OBJECT_xxx guards
2. Includes in BACnetArduino.h are properly guarded
3. No code paths attempt to use disabled objects
4. BACnetConfig.h properly defines tier-based object enablement
5. Feature flags (COV, Priority Arrays) are properly guarded

Usage:
    python verify_conditional.py [library_directory]

If no directory is specified, uses the parent directory of this script.
"""

import os
import sys
import re
from pathlib import Path

# ANSI color codes for output
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def print_header(text):
    """Print section header"""
    print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.BLUE}{text}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}\n")

def print_check(description, passed, details=None):
    """Print check result"""
    status = f"{Colors.GREEN}✓ PASS{Colors.RESET}" if passed else f"{Colors.RED}✗ FAIL{Colors.RESET}"
    print(f"{status} - {description}")
    if details:
        print(f"      {details}")
    return passed

def check_config_definitions(lib_dir):
    """Check BACnetConfig.h defines object flags properly"""
    print_header("1. BACnetConfig.h Object Definitions Check")
    
    config_file = lib_dir / 'src' / 'BACnetConfig.h'
    if not config_file.exists():
        print_check("BACnetConfig.h exists", False, "File not found")
        return False
    
    with open(config_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check for required object definitions
    required_objects = [
        'BACNET_OBJECT_DEVICE',
        'BACNET_OBJECT_BINARY_VALUE',
        'BACNET_OBJECT_ANALOG_VALUE',
        'BACNET_OBJECT_BINARY_OUTPUT',
        'BACNET_OBJECT_ANALOG_INPUT',
    ]
    
    all_passed = True
    for obj in required_objects:
        # Check if defined
        pattern = rf'#define\s+{obj}\s+[01]'
        found = re.search(pattern, content)
        passed = found is not None
        all_passed &= print_check(
            f"{obj} is defined",
            passed,
            f"Found: {found.group(0)}" if passed else "Not defined"
        )
    
    # Check tier-based enablement structure
    tier2_check = re.search(r'#if\s+BOARD_TIER\s*>=\s*2.*?#define\s+BACNET_OBJECT_BINARY_OUTPUT', 
                           content, re.DOTALL)
    passed = tier2_check is not None
    all_passed &= print_check(
        "Tier 2+ objects are conditionally defined",
        passed,
        "Found tier-based object enablement" if passed else "Missing tier structure"
    )
    
    # Check feature flags
    feature_flags = ['BACNET_FEATURE_COV', 'BACNET_FEATURE_PRIORITY_ARRAY']
    for flag in feature_flags:
        pattern = rf'#define\s+{flag}\s+[01]'
        found = re.search(pattern, content)
        passed = found is not None
        all_passed &= print_check(
            f"{flag} is defined",
            passed,
            f"Found: {found.group(0)}" if passed else "Not defined"
        )
    
    return all_passed

def check_wrapper_guards(lib_dir):
    """Check that wrapper files have proper conditional guards"""
    print_header("2. Wrapper File Conditional Guards Check")
    
    src_dir = lib_dir / 'src'
    
    # Map of files to their expected guard macro
    wrapper_files = {
        'BACnetBinaryValue.h': 'BACNET_OBJECT_BINARY_VALUE',
        'BACnetBinaryValue.cpp': 'BACNET_OBJECT_BINARY_VALUE',
        'BACnetAnalogValue.h': 'BACNET_OBJECT_ANALOG_VALUE',
        'BACnetAnalogValue.cpp': 'BACNET_OBJECT_ANALOG_VALUE',
    }
    
    all_passed = True
    for filename, guard_macro in wrapper_files.items():
        file_path = src_dir / filename
        
        if not file_path.exists():
            print_check(f"{filename} exists", False, "File not found")
            all_passed = False
            continue
        
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for opening guard
        opening_guard = re.search(rf'#if\s+{guard_macro}', content)
        passed = opening_guard is not None
        all_passed &= print_check(
            f"{filename} has opening #{guard_macro} guard",
            passed,
            f"Line: {content[:opening_guard.end()].count(chr(10)) + 1}" if passed else "Missing opening guard"
        )
        
        # Check for closing guard (should be near end)
        closing_guard = re.search(rf'#endif\s*//\s*{guard_macro}', content)
        passed = closing_guard is not None
        all_passed &= print_check(
            f"{filename} has closing #{guard_macro} guard",
            passed,
            f"Line: {content[:closing_guard.end()].count(chr(10)) + 1}" if passed else "Missing closing guard"
        )
    
    return all_passed

def check_main_header_guards(lib_dir):
    """Check BACnetArduino.h has conditional includes"""
    print_header("3. BACnetArduino.h Conditional Includes Check")
    
    header_file = lib_dir / 'src' / 'BACnetArduino.h'
    if not header_file.exists():
        print_check("BACnetArduino.h exists", False, "File not found")
        return False
    
    with open(header_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check for conditional includes
    includes_to_check = [
        ('BACnetBinaryValue.h', 'BACNET_OBJECT_BINARY_VALUE'),
        ('BACnetAnalogValue.h', 'BACNET_OBJECT_ANALOG_VALUE'),
        ('BACnetBinaryOutput.h', 'BACNET_OBJECT_BINARY_OUTPUT'),
        ('BACnetAnalogInput.h', 'BACNET_OBJECT_ANALOG_INPUT'),
    ]
    
    all_passed = True
    for include_file, guard_macro in includes_to_check:
        # Look for pattern: #if GUARD ... #include "file.h" ... #endif
        pattern = rf'#if\s+{guard_macro}.*?#include\s+"({include_file})"'
        found = re.search(pattern, content, re.DOTALL)
        passed = found is not None
        all_passed &= print_check(
            f"{include_file} include is guarded by #{guard_macro}",
            passed,
            f"Found conditional include" if passed else "Missing or unguarded include"
        )
    
    return all_passed

def check_feature_guards(lib_dir):
    """Check that COV and other features are properly guarded"""
    print_header("4. Feature Guards Check (COV, Priority Arrays)")
    
    src_dir = lib_dir / 'src'
    
    # Files that should have COV guards
    files_with_cov = [
        'BACnetBinaryValue.h',
        'BACnetBinaryValue.cpp',
        'BACnetAnalogValue.h',
        'BACnetAnalogValue.cpp',
    ]
    
    all_passed = True
    for filename in files_with_cov:
        file_path = src_dir / filename
        
        if not file_path.exists():
            continue
        
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for COV-related code
        has_cov_code = re.search(r'(enableCOV|disableCOV|_cov_enabled)', content)
        
        if has_cov_code:
            # Check that COV code is guarded
            guarded = re.search(r'#if\s+BACNET_FEATURE_COV.*?enableCOV', content, re.DOTALL)
            passed = guarded is not None
            all_passed &= print_check(
                f"{filename} COV code is guarded by #if BACNET_FEATURE_COV",
                passed,
                "COV properly guarded" if passed else "COV code exists but not guarded"
            )
    
    return all_passed

def check_no_unguarded_usage(lib_dir):
    """Check that there are no attempts to use objects without guards"""
    print_header("5. Unguarded Object Usage Check")
    
    src_dir = lib_dir / 'src'
    
    # Files to skip (these are the object definitions themselves)
    skip_files = {
        'BACnetBinaryValue.h', 'BACnetBinaryValue.cpp',
        'BACnetAnalogValue.h', 'BACnetAnalogValue.cpp',
        'BACnetArduino.h',  # Already checked separately
        'BACnetConfig.h',    # Defines the flags
    }
    
    # Search for potential unguarded usage
    all_passed = True
    for cpp_file in src_dir.glob('*.cpp'):
        if cpp_file.name in skip_files:
            continue
        
        with open(cpp_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Look for BACnetBinaryValue/BACnetAnalogValue usage
        # This is a simple check - real usage should be behind guards
        if 'BACnetBinaryValue' in content or 'BACnetAnalogValue' in content:
            # Check if file has appropriate guards
            has_guards = re.search(r'#if\s+BACNET_OBJECT_(BINARY_VALUE|ANALOG_VALUE)', content)
            passed = has_guards is not None
            all_passed &= print_check(
                f"{cpp_file.name} uses wrapper objects with proper guards",
                passed,
                "Guarded usage found" if passed else "Potential unguarded usage"
            )
    
    return all_passed

def check_example_compatibility(lib_dir):
    """Check that examples only use available objects"""
    print_header("6. Example Sketch Compatibility Check")
    
    examples_dir = lib_dir / 'examples'
    if not examples_dir.exists():
        print_check("examples/ directory exists", False, "No examples to check")
        return True  # Not a failure if no examples yet
    
    ino_files = list(examples_dir.rglob('*.ino'))
    if len(ino_files) == 0:
        print(f"{Colors.YELLOW}⚠ INFO{Colors.RESET} - No example sketches found yet")
        return True
    
    all_passed = True
    for ino_file in ino_files:
        with open(ino_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check if example uses Tier 2+ objects
        uses_tier2 = re.search(r'BACnet(BinaryOutput|AnalogInput|MultiStateValue)', content)
        
        if uses_tier2:
            # Should have a comment or #if guard warning about tier requirements
            has_warning = re.search(r'(Tier 2|Mega|Due|ESP32)', content, re.IGNORECASE)
            passed = has_warning is not None
            all_passed &= print_check(
                f"{ino_file.name} using Tier 2+ objects has tier warning",
                passed,
                "Warning found" if passed else "Missing tier requirement warning"
            )
    
    return all_passed

def main():
    """Main verification function"""
    # Determine library directory
    if len(sys.argv) > 1:
        lib_dir = Path(sys.argv[1]).resolve()
    else:
        # Use parent directory of this script
        lib_dir = Path(__file__).parent.parent.resolve()
    
    print(f"{Colors.BOLD}BACnet for Arduino - Conditional Compilation Verification{Colors.RESET}")
    print(f"Library directory: {lib_dir}\n")
    
    if not lib_dir.exists():
        print(f"{Colors.RED}Error: Directory does not exist: {lib_dir}{Colors.RESET}")
        return 1
    
    # Run all checks
    results = []
    results.append(check_config_definitions(lib_dir))
    results.append(check_wrapper_guards(lib_dir))
    results.append(check_main_header_guards(lib_dir))
    results.append(check_feature_guards(lib_dir))
    results.append(check_no_unguarded_usage(lib_dir))
    results.append(check_example_compatibility(lib_dir))
    
    # Summary
    print_header("Verification Summary")
    total_checks = len(results)
    passed_checks = sum(results)
    
    if all(results):
        print(f"{Colors.GREEN}{Colors.BOLD}✓ ALL CHECKS PASSED ({passed_checks}/{total_checks}){Colors.RESET}")
        print(f"\n{Colors.GREEN}Conditional compilation system is properly implemented!{Colors.RESET}\n")
        return 0
    else:
        print(f"{Colors.YELLOW}{Colors.BOLD}⚠ SOME CHECKS FAILED ({passed_checks}/{total_checks}){Colors.RESET}")
        print(f"\n{Colors.YELLOW}Please fix the issues above to ensure proper tier-based compilation.{Colors.RESET}\n")
        return 1

if __name__ == '__main__':
    sys.exit(main())
