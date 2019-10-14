package org.eclipse.p4lang.snippet;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.lsp4e.LanguageServiceAccessor.LSPDocumentInfo;
import org.eclipse.lsp4e.operations.completion.LSCompletionProposal;
import org.eclipse.lsp4j.CompletionItem;
import org.eclipse.lsp4j.CompletionItemKind;
import org.eclipse.lsp4j.InsertTextFormat;
import org.eclipse.lsp4j.Position;
import org.eclipse.lsp4j.Range;
import org.eclipse.lsp4j.TextEdit;

@SuppressWarnings("restriction")
public class Snippet {
	private String[] replacementLines;
	private String display;
	private CompletionItemKind kind;

	public Snippet(String display, CompletionItemKind kind, String[] replacementLines) {
		this.replacementLines = replacementLines;
		this.display = display;
		this.kind = kind;
	}

	public ICompletionProposal convertToCompletionProposal(int offset, LSPDocumentInfo info, String prefix,
			String lineIndentation) {
		CompletionItem item = new CompletionItem();
		item.setLabel(display);
		item.setKind(kind);
		item.setInsertTextFormat(InsertTextFormat.Snippet);

		Range r = null;
		try {
			int line = info.getDocument().getLineOfOffset(offset);
			int lineOffset = offset - info.getDocument().getLineOffset(line);
			r = new Range(new Position(line, lineOffset - prefix.length()), new Position(line, lineOffset));
		} catch (BadLocationException e) {
			// Caught by null return
		}
		if (r == null) {
			return null;
		}
		item.setTextEdit(new TextEdit(r, createReplacement(lineIndentation)));
		return new LSCompletionProposal(info.getDocument(), offset, item, null);
	}

	public boolean matchesPrefix(String prefix) {
		// TODO: expand matching if not extensive enough
		return this.display.startsWith(prefix);
	}

	private String createReplacement(String lineIndentation) {
		StringBuilder responseBuilder = new StringBuilder();

		if (replacementLines.length == 1) {
			return replacementLines[0];
		} else if (replacementLines.length > 1) {
			for (String line : replacementLines) {
				if (responseBuilder.length() == 0) {
					responseBuilder.append(line);
					continue;
				}
				responseBuilder.append('\n');
				responseBuilder.append(lineIndentation);
				responseBuilder.append(line);
			}
		}
		return responseBuilder.toString();
	}
}
