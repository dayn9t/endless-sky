mod types;
mod extractor;
mod translator;
mod generator;

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
    }

    Ok(())
}
