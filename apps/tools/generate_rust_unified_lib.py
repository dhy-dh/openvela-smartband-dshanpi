#!/usr/bin/env python3

"""
This script generates a unified Rust wrapper crate by combining multiple Rust crates into a single static library.
It can parse crate information from command line arguments.
The script creates a Cargo.toml and src/lib.rs that re-export all specified crates.

Workflow:
1. Parse command line arguments to determine crate sources:
   - Use --crate-name/--crate-path for explicit lists
   - Use --crate for combined format (name:path)
2. Validate and collect crate information (name and path for each crate)
3. Create the output directory (default: ./rust_unified_lib or as specified)
4. Generate Cargo.toml listing all crates as dependencies
5. Generate src/lib.rs re-exporting all crates as modules
6. Generate .cargo/config.toml file from RUST_UNIFIED_LIB_CONFIG environment variable
7. Print status messages for each step
8. Exit with status code 0 on success, 1 on error

Usage:
    python generate_rust_unified_lib.py --crate name:path [--crate ...] [--output-dir DIR]
    python generate_rust_unified_lib.py --crate-name NAME --crate-path PATH [--output-dir DIR]

Environment Variables:
    RUST_UNIFIED_LIB_CONFIG: Content to write to the .cargo/config.toml file (optional)
"""

import argparse
import os
import sys
from pathlib import Path


def split_and_strip(items):
    """
    Split a string or list of strings by semicolon and strip whitespace from each item.
    Returns a flat list of non-empty, stripped strings.
    """
    if isinstance(items, str):
        items = items.split(";")
    result = []
    for item in items:
        if isinstance(item, str):
            result.extend([x.strip() for x in item.split(";") if x.strip()])
        else:
            result.append(str(item).strip())
    return result


def parse_crates_from_args(crate_names=None, crate_paths=None):
    """
    Parse crate information from separate command line arguments for names and paths.
    Returns a list of dicts with 'name' and 'path' for each crate.
    """
    if not crate_names or not crate_paths:
        return []

    crate_names = split_and_strip(crate_names)
    crate_paths = split_and_strip(crate_paths)

    if len(crate_names) != len(crate_paths):
        print("Warning: Number of crate names and paths don't match")
        return []

    return [
        {"name": name, "path": path} for name, path in zip(crate_names, crate_paths)
    ]


def create_cargo_toml(crate_dir, crate_info):
    """
    Create Cargo.toml in the output directory, listing all crates as dependencies.
    """
    cargo_toml_path = crate_dir / "Cargo.toml"

    # Start with basic crate information
    cargo_content = """[package]
name = "rust_unified_lib"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]

[profile.dev]
panic = "abort"

[profile.release]
lto = true
codegen-units = 1
opt-level = "z"
debug = true
panic = "abort"

[dependencies]
"""

    # Add each crate as a dependency
    for crate in crate_info:
        crate_name = crate["name"]
        crate_path = crate["path"]
        cargo_content += f'{crate_name} = {{ path = "{crate_path}" }}\n'

    with open(cargo_toml_path, "w") as f:
        f.write(cargo_content)

    print(f"Created {cargo_toml_path}")


def create_lib_rs(crate_dir, crate_info):
    """
    Create src/lib.rs in the output directory, re-exporting all crates as modules.
    """
    lib_rs_dir = crate_dir / "src"
    lib_rs_dir.mkdir(exist_ok=True)

    lib_rs_path = lib_rs_dir / "lib.rs"

    lib_content = "// Automatically generated unified Rust library\n\n"

    # Add use statements and re-exports for each crate
    for crate in crate_info:
        crate_name = crate["name"]
        # Convert hyphens to underscores for module re-export
        module_name = crate_name.replace("-", "_")
        lib_content += f"// Re-exporting {crate_name} as {module_name}\n"
        lib_content += f"pub use {module_name};\n\n"

    with open(lib_rs_path, "w") as f:
        f.write(lib_content)

    print(f"Created {lib_rs_path}")


def create_config_file(crate_dir):
    """
    Create .cargo/config.toml file in the output directory from RUST_UNIFIED_LIB_CONFIG environment variable.
    """
    # Create .cargo directory
    cargo_dir = crate_dir / ".cargo"
    cargo_dir.mkdir(exist_ok=True)

    # Create config.toml in .cargo directory
    config_path = cargo_dir / "config.toml"

    config_content = ""

    # Check for direct config.toml content in environment variable
    if "RUST_UNIFIED_LIB_CONFIG" in os.environ:
        config_content = os.environ["RUST_UNIFIED_LIB_CONFIG"]
    else:
        print(
            "Info: RUST_UNIFIED_LIB_CONFIG environment variable not found, using minimal config.toml"
        )
        config_content = "# Minimal config.toml file for Rust unified library\n"

    # Add source configuration for vendored dependencies
    script_dir = Path(__file__).parent
    registry_dir = script_dir.parent.parent / "external" / "rust" / "registry"

    config_content = f"""{config_content}

[source.crates-io]
replace-with = "vendored-sources"

[source.vendored-sources]
directory = "{registry_dir}"

[net]
offline = true
"""

    with open(config_path, "w") as f:
        f.write(config_content)

    print(f"Created {config_path}")


def main():
    """
    Main entry point for the script. Parses arguments, gathers crate info, and generates unified Rust crate files.
    """
    parser = argparse.ArgumentParser(
        description="Generate a unified Rust wrapper crate"
    )
    parser.add_argument(
        "--crate",
        action="append",
        help="Crate information in format 'name:path' (can be specified multiple times)",
    )
    parser.add_argument(
        "--crate-name",
        action="append",
        help="Crate name (can be specified multiple times)",
    )
    parser.add_argument(
        "--crate-path",
        action="append",
        help="Crate path (can be specified multiple times)",
    )
    parser.add_argument(
        "--output-dir",
        default="./rust_unified_lib",
        help="Output directory for the unified crate",
    )

    args = parser.parse_args()

    # Get crate information from command line
    if args.crate_name and args.crate_path:
        crate_info = parse_crates_from_args(
            crate_names=args.crate_name, crate_paths=args.crate_path
        )
    elif args.crate:
        # Handle the old format by parsing crate names and paths from the combined format
        crate_names = []
        crate_paths = []
        for crate_arg in args.crate:
            # Split by comma to handle multiple crates in one argument
            crate_list = crate_arg.split(",")
            for crate_str in crate_list:
                if ":" in crate_str:
                    name, path = crate_str.split(":", 1)
                    crate_names.append(name.strip())
                    crate_paths.append(path.strip())
        crate_info = parse_crates_from_args(
            crate_names=crate_names, crate_paths=crate_paths
        )
    else:
        print("Error: Either --crate-name/--crate-path or --crate must be specified")
        return 1

    if not crate_info:
        print("No crate information found. Exiting.")
        return 1

    # Create output directory
    crate_dir = Path(args.output_dir)
    crate_dir.mkdir(parents=True, exist_ok=True)

    # Create Cargo.toml
    create_cargo_toml(crate_dir, crate_info)

    # Create .cargo/config.toml file
    create_config_file(crate_dir)

    # Create src/lib.rs
    create_lib_rs(crate_dir, crate_info)

    print(f"Successfully created unified Rust crate in {crate_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
