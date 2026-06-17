#include "swap_corrector.hpp"
#include "../validation/metadata_value_validator.hpp"

namespace kivo::metadata {

FieldSwapCandidate FieldSwapCorrector::formalize(const FieldSwapCandidate& candidate) {
    FieldSwapCandidate result = candidate;

    if (result.issueType != MetadataIssueType::PossibleTitleArtistSwap) {
        return result;
    }

    // Safety gate: the suggested artist must not be empty or a placeholder.
    if (!MetadataValueValidator::isValidArtist(result.suggestedArtist)) {
        result.issueType = MetadataIssueType::InvalidMetadata;
        result.rejectReasons << QStringLiteral(
            "Suggested artist would be empty or a placeholder; swap rejected");
        result.suggestedArtist = result.originalArtist;
        result.suggestedTitle = result.originalTitle;
        result.confidence = 0.0;
        result.requiresReview = true;
        return result;
    }

    // Safety gate: the suggested title must not be empty or a placeholder.
    if (!MetadataValueValidator::isValidTitle(result.suggestedTitle)) {
        result.issueType = MetadataIssueType::InvalidMetadata;
        result.rejectReasons << QStringLiteral(
            "Suggested title would be empty or a placeholder; swap rejected");
        result.suggestedArtist = result.originalArtist;
        result.suggestedTitle = result.originalTitle;
        result.confidence = 0.0;
        result.requiresReview = true;
        return result;
    }

    return result;
}

QList<FieldSwapCandidate> FieldSwapCorrector::formalizeBatch(const QList<FieldSwapCandidate>& candidates) {
    QList<FieldSwapCandidate> result;
    result.reserve(candidates.size());
    for (const auto& candidate : candidates) {
        result.append(formalize(candidate));
    }
    return result;
}

}  // namespace kivo::metadata
