// Tokenizer
package cs149.ngram;

import java.util.Iterator;

public class Tokenizer implements Iterator<String> {
    private final String _src;
    private int _pos;
    private String _avail;

    public Tokenizer(final String src) {
        _src = src;
        _pos = 0;
        advance();
    }

    private static boolean isSentenceEnd(final char ch) {
        return ch == '.' || ch == '!' || ch == '?';
    }

    private void advance() {
        final int n = _src.length();

        // skip non-alphanumeric and non-sentence-ending characters
        char ch = '\0';
        while (_pos < n) {
            ch = _src.charAt(_pos);
            if (isSentenceEnd(ch) || Character.isLetterOrDigit(_src.charAt(_pos))) {
                break;
            }
            ++_pos;
        }

        // gobble alphanumeric characters or period, whichever we found first
        if (isSentenceEnd(ch)) {
            ++_pos;
            while (_pos < n && isSentenceEnd(_src.charAt(_pos))) {
                ++_pos;
            }
            _avail = "#";
        } else {
            final int begin = _pos;
            while (_pos < n && Character.isLetterOrDigit(_src.charAt(_pos))) {
                ++_pos;
            }
            _avail = (_pos == begin) ? null : _src.substring(begin, _pos).toLowerCase();
        }
    }

    public boolean hasNext() {
        return _avail != null;
    }

    //returns a word or a sentence separator.
    public String next() {
        if (!hasNext()) {
            throw new IllegalStateException();
        }
        final String result = _avail;
        advance();
        return result;
    }

    public void remove() {
        throw new UnsupportedOperationException();
    }
}
