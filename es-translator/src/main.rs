mod types;
mod extractor;
mod translator;
mod generator;
mod batch_splitter;
mod scanner;

use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(name = "es-translator")]
#[command(about = "Endless Sky AI-powered translation tool", long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Extract translatable text from source files
    Extract {
        /// Source type: ui or data
        #[arg(short, long)]
        source: String,
        /// Output file path
        #[arg(short, long, default_value = "pending.json")]
        output: String,
    },
    /// Translate extracted text using Qwen API
    Translate {
        /// Input file path
        #[arg(short, long)]
        input: String,
        /// Output file path
        #[arg(short, long, default_value = "translated.json")]
        output: String,
    },
    /// Generate translation files
    Generate {
        /// Type: ui or data
        #[arg(short, long)]
        r#type: String,
        /// Input file path
        #[arg(short, long)]
        input: String,
    },
    /// Run full pipeline
    Run {
        /// Translate all (ui + data)
        #[arg(long)]
        all: bool,
    },
    /// Split large file into chunks
    Split {
        /// Input file path
        #[arg(short, long)]
        input: String,
        /// Output directory for chunks
        #[arg(short, long, default_value = "chunks")]
        output_dir: String,
    },
    /// Process all chunks in a directory
    ProcessChunks {
        /// Input directory with chunk files
        #[arg(short, long)]
        input_dir: String,
        /// Output directory for translated chunks
        #[arg(short, long, default_value = "translated_chunks")]
        output_dir: String,
    },
    /// Merge translated chunks
    Merge {
        /// Directory with translated chunks
        #[arg(short, long)]
        input_dir: String,
        /// Final output file
        #[arg(short, long)]
        output: String,
        /// Source identifier
        #[arg(short, long)]
        source: String,
    },
    /// Scan for untranslated content by comparing data/ with plugin
    Scan {
        /// Original data directory
        #[arg(long, default_value = "../data")]
        data_dir: String,
        /// Plugin data directory
        #[arg(long, default_value = "../plugins/zh_CN/data")]
        plugin_dir: String,
        /// Output pending JSON file
        #[arg(short, long, default_value = "pending_scan.json")]
        output: String,
        /// Print human-readable report to stdout
        #[arg(long)]
        report: bool,
    },
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    dotenv::dotenv().ok();
    tracing_subscriber::fmt::init();

    let cli = Cli::parse();

    match cli.command {
        Commands::Extract { source, output } => {
            extractor::extract(&source, &output)?;
        }
        Commands::Translate { input, output } => {
            translator::translate(&input, &output).await?;
        }
        Commands::Generate { r#type, input } => {
            generator::generate(&r#type, &input)?;
        }
        Commands::Run { all } => {
            if all {
                // TODO: Full pipeline
            }
        }
        Commands::Split { input, output_dir } => {
            batch_splitter::split_pending_file(&input, &output_dir)?;
        }
        Commands::ProcessChunks { input_dir, output_dir } => {
            process_all_chunks(&input_dir, &output_dir).await?;
        }
        Commands::Merge { input_dir, output, source } => {
            let chunk_files: Vec<String> = std::fs::read_dir(&input_dir)?
                .filter_map(|e| e.ok())
                .filter(|e| e.path().extension().and_then(|s| s.to_str()) == Some("json"))
                .filter_map(|e| e.path().to_str().map(String::from))
                .collect();
            batch_splitter::merge_translated_chunks(&chunk_files, &output, &source)?;
        }
        Commands::Scan { data_dir, plugin_dir, output, report } => {
            scanner::scan(scanner::ScanOptions {
                data_dir: std::path::PathBuf::from(data_dir),
                plugin_dir: std::path::PathBuf::from(plugin_dir),
                output,
                report,
            })?;
        }
    }

    Ok(())
}

async fn process_all_chunks(input_dir: &str, output_dir: &str) -> anyhow::Result<()> {
    use std::path::Path;

    std::fs::create_dir_all(output_dir)?;

    let mut chunk_files: Vec<String> = std::fs::read_dir(input_dir)?
        .filter_map(|e| e.ok())
        .filter(|e| e.path().extension().and_then(|s| s.to_str()) == Some("json"))
        .filter_map(|e| e.path().to_str().map(String::from))
        .collect();

    chunk_files.sort();

    let total = chunk_files.len();
    let completed = batch_splitter::count_completed(output_dir)?;

    println!("Found {} chunks to process", total);
    println!("Already completed: {} items", completed);

    for (idx, chunk_file) in chunk_files.iter().enumerate() {
        let filename = Path::new(chunk_file).file_name().unwrap().to_str().unwrap();
        let output_file = format!("{}/{}", output_dir, filename);

        if Path::new(&output_file).exists() {
            println!("[{}/{}] {} already translated, skipping", idx + 1, total, filename);
            continue;
        }

        println!("[{}/{}] Translating {}...", idx + 1, total, filename);

        match translator::translate(chunk_file, &output_file).await {
            Ok(_) => println!("  ✓ Completed: {}", output_file),
            Err(e) => {
                eprintln!("  ✗ Failed: {}", e);
                eprintln!("  Continuing with next chunk...");
            }
        }

        // Longer delay between chunks to avoid rate limiting
        tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
    }

    let final_count = batch_splitter::count_completed(output_dir)?;
    println!("\nBatch processing complete: {}/{} chunks, {} total items", total, total, final_count);

    Ok(())
}
