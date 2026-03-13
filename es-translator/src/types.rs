use serde::{Deserialize, Serialize};

/// A single translatable item
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TranslateItem {
    /// Unique identifier (e.g., "outfits.show_for_sale")
    pub id: String,
    /// Original English text
    pub text: String,
    /// Context hint for translator
    #[serde(skip_serializing_if = "Option::is_none")]
    pub context: Option<String>,
    /// Source file path
    #[serde(skip_serializing_if = "Option::is_none")]
    pub source: Option<String>,
}

/// Collection of items to translate
#[derive(Debug, Serialize, Deserialize)]
pub struct PendingTranslations {
    pub source: String,  // "ui" or "data"
    pub items: Vec<TranslateItem>,
}

/// Collection of translated items
#[derive(Debug, Serialize, Deserialize)]
pub struct TranslatedItems {
    pub source: String,
    pub items: Vec<TranslatedItem>,
}

/// A translated item
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TranslatedItem {
    pub id: String,
    pub original: String,
    pub translated: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub source: Option<String>,
}

/// Qwen API request
#[derive(Debug, Serialize)]
pub struct QwenRequest {
    pub model: String,
    pub messages: Vec<Message>,
}

#[derive(Debug, Serialize)]
pub struct Message {
    pub role: String,
    pub content: String,
}

/// Qwen API response
#[derive(Debug, Deserialize)]
pub struct QwenResponse {
    pub choices: Vec<QwenChoice>,
}

#[derive(Debug, Deserialize)]
pub struct QwenChoice {
    pub message: QwenMessage,
}

#[derive(Debug, Deserialize)]
pub struct QwenMessage {
    pub content: String,
}
