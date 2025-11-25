#!/usr/bin/env python3
"""
Verify Arduino Library Structure

This script validates that the BACnet for Arduino library follows the
Arduino Library Specification 1.5.

Checks:
1. Required files exist (library.properties, keywords.txt)
2. Required directories exist (src/, examples/)
3. Source files are in correct locations
4. No source files in root directory
5. Example structure is valid
6. Metadata files are properly formatted

Usage:
    python verify_structure.py [library_directory]

If no directory is specified, uses the parent directory of this script.
"""

import os
import sys
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

def check_required_files(lib_dir):
    """Check that required files exist"""
    print_header("1. Required Files Check")
    
    required_files = {
        'library.properties': 'Arduino Library Manager metadata',
        'keywords.txt': 'Syntax highlighting definitions',
        'LICENSE': 'License information',
        'README.md': 'Library documentation'
    }
    
    all_passed = True
    for filename, description in required_files.items():
        file_path = lib_dir / filename
        passed = file_path.exists()
        all_passed &= print_check(
            f"{filename} exists ({description})",
            passed,
            f"Path: {file_path}" if passed else f"Missing: {file_path}"
        )
    
    return all_passed

def check_required_directories(lib_dir):
    """Check that required directories exist"""
    print_header("2. Required Directories Check")
    
    required_dirs = {
        'src': 'Source code directory',
        'examples': 'Example sketches directory',
        'extras': 'Additional documentation (optional)'
    }
    
    all_passed = True
    for dirname, description in required_dirs.items():
        dir_path = lib_dir / dirname
        passed = dir_path.exists() and dir_path.is_dir()
        all_passed &= print_check(
            f"{dirname}/ exists ({description})",
            passed,
            f"Path: {dir_path}" if passed else f"Missing: {dir_path}"
        )
    
    return all_passed

def check_source_location(lib_dir):
    """Check that source files are in correct location"""
    print_header("3. Source File Location Check")
    
    # Check for .cpp, .h files in root (should not exist)
    root_sources = list(lib_dir.glob('*.cpp')) + list(lib_dir.glob('*.h'))
    root_sources = [f for f in root_sources if f.is_file()]
    
    # Exclude .ino files (Arduino sketches are allowed in root for single-file libraries)
    root_sources = [f for f in root_sources if f.suffix != '.ino']
    
    all_passed = True
    passed = len(root_sources) == 0
    all_passed &= print_check(
        "No source files (.h, .cpp) in root directory",
        passed,
        f"Found: {', '.join(f.name for f in root_sources)}" if not passed else "All source files in src/"
    )
    
    # Check that src/ contains source files
    src_dir = lib_dir / 'src'
    if src_dir.exists():
        src_files = list(src_dir.rglob('*.h')) + list(src_dir.rglob('*.cpp'))
        passed = len(src_files) > 0
        all_passed &= print_check(
            "src/ directory contains source files",
            passed,
            f"Found {len(src_files)} source files" if passed else "No source files in src/"
        )
    
    return all_passed

def check_library_properties(lib_dir):
    """Validate library.properties format"""
    print_header("4. library.properties Validation")
    
    props_file = lib_dir / 'library.properties'
    if not props_file.exists():
        print_check("library.properties exists", False, "File not found")
        return False
    
    required_fields = ['name', 'version', 'author', 'maintainer', 'sentence', 
                      'paragraph', 'category', 'url', 'architectures']
    
    found_fields = {}
    with open(props_file, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if '=' in line and not line.startswith('#'):
                key, value = line.split('=', 1)
                found_fields[key.strip()] = value.strip()
    
    all_passed = True
    for field in required_fields:
        passed = field in found_fields and found_fields[field]
        all_passed &= print_check(
            f"Field '{field}' present",
            passed,
            f"Value: {found_fields.get(field, 'N/A')[:50]}..." if passed else "Missing or empty"
        )
    
    # Check version format (x.y.z)
    if 'version' in found_fields:
        version = found_fields['version']
        parts = version.split('.')
        passed = len(parts) == 3 and all(p.isdigit() for p in parts)
        all_passed &= print_check(
            "Version format is semantic (x.y.z)",
            passed,
            f"Version: {version}"
        )
    
    return all_passed

def check_keywords_txt(lib_dir):
    """Validate keywords.txt format"""
    print_header("5. keywords.txt Validation")
    
    keywords_file = lib_dir / 'keywords.txt'
    if not keywords_file.exists():
        print_check("keywords.txt exists", False, "File not found")
        return False
    
    with open(keywords_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    all_passed = True
    
    # Check that file is not empty
    non_empty_lines = [l for l in lines if l.strip() and not l.strip().startswith('#')]
    passed = len(non_empty_lines) > 0
    all_passed &= print_check(
        "keywords.txt is not empty",
        passed,
        f"Found {len(non_empty_lines)} keyword lines"
    )
    
    # Check for TAB delimiter (not spaces)
    tab_errors = []
    for i, line in enumerate(non_empty_lines[:10], 1):  # Check first 10 lines
        if '\t' not in line and not line.startswith('#'):
            tab_errors.append(f"Line {i}: '{line.strip()}'")
    
    passed = len(tab_errors) == 0
    all_passed &= print_check(
        "Keywords use TAB delimiter",
        passed,
        "All keywords properly formatted" if passed else f"Errors: {tab_errors[0]}"
    )
    
    # Check for valid keyword types (KEYWORD1, KEYWORD2, LITERAL1, etc.)
    valid_types = ['KEYWORD1', 'KEYWORD2', 'KEYWORD3', 'LITERAL1', 'LITERAL2']
    type_errors = []
    for i, line in enumerate(non_empty_lines[:10], 1):
        parts = line.split('\t')
        if len(parts) >= 2 and parts[1].strip() not in valid_types:
            type_errors.append(f"Line {i}: Invalid type '{parts[1].strip()}'")
    
    passed = len(type_errors) == 0
    all_passed &= print_check(
        "Keywords use valid types",
        passed,
        "All types are valid" if passed else f"Errors: {type_errors[0]}"
    )
    
    return all_passed

def check_examples(lib_dir):
    """Validate examples directory structure"""
    print_header("6. Examples Structure Check")
    
    examples_dir = lib_dir / 'examples'
    if not examples_dir.exists():
        print_check("examples/ directory exists", False, "Directory not found")
        return False
    
    # Find all .ino files in examples/
    ino_files = list(examples_dir.rglob('*.ino'))
    
    all_passed = True
    passed = len(ino_files) > 0
    all_passed &= print_check(
        "Examples directory contains .ino sketches",
        passed,
        f"Found {len(ino_files)} example sketches" if passed else "No sketches found"
    )
    
    # Check that each .ino is in a folder with the same name
    structure_errors = []
    for ino_file in ino_files[:5]:  # Check first 5
        parent_name = ino_file.parent.name
        sketch_name = ino_file.stem
        if parent_name != sketch_name:
            structure_errors.append(f"{ino_file.name} should be in {sketch_name}/ folder")
    
    passed = len(structure_errors) == 0
    all_passed &= print_check(
        "Examples follow naming convention (sketch.ino in sketch/ folder)",
        passed,
        "All examples properly structured" if passed else f"Errors: {structure_errors[0]}"
    )
    
    return all_passed

def check_bacnet_stack(lib_dir):
    """Check BACnet stack integration"""
    print_header("7. BACnet Stack Integration Check")
    
    bacnet_dir = lib_dir / 'src' / 'bacnet'
    
    all_passed = True
    passed = bacnet_dir.exists() and bacnet_dir.is_dir()
    all_passed &= print_check(
        "src/bacnet/ directory exists",
        passed,
        f"Path: {bacnet_dir}" if passed else "BACnet stack not found"
    )
    
    if passed:
        # Count source files in bacnet/
        bacnet_files = list(bacnet_dir.rglob('*.c')) + list(bacnet_dir.rglob('*.h'))
        passed = len(bacnet_files) > 100  # Should have many files
        all_passed &= print_check(
            "BACnet stack contains source files",
            passed,
            f"Found {len(bacnet_files)} files" if passed else "Insufficient files"
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
    
    print(f"{Colors.BOLD}BACnet for Arduino - Library Structure Verification{Colors.RESET}")
    print(f"Library directory: {lib_dir}\n")
    
    if not lib_dir.exists():
        print(f"{Colors.RED}Error: Directory does not exist: {lib_dir}{Colors.RESET}")
        return 1
    
    # Run all checks
    results = []
    results.append(check_required_files(lib_dir))
    results.append(check_required_directories(lib_dir))
    results.append(check_source_location(lib_dir))
    results.append(check_library_properties(lib_dir))
    results.append(check_keywords_txt(lib_dir))
    results.append(check_examples(lib_dir))
    results.append(check_bacnet_stack(lib_dir))
    
    # Summary
    print_header("Verification Summary")
    total_checks = len(results)
    passed_checks = sum(results)
    
    if all(results):
        print(f"{Colors.GREEN}{Colors.BOLD}✓ ALL CHECKS PASSED ({passed_checks}/{total_checks}){Colors.RESET}")
        print(f"\n{Colors.GREEN}Library structure is valid and ready for Arduino Library Manager!{Colors.RESET}\n")
        return 0
    else:
        print(f"{Colors.YELLOW}{Colors.BOLD}⚠ SOME CHECKS FAILED ({passed_checks}/{total_checks}){Colors.RESET}")
        print(f"\n{Colors.YELLOW}Please fix the issues above before submitting to Arduino Library Manager.{Colors.RESET}\n")
        return 1

if __name__ == '__main__':
    sys.exit(main())
