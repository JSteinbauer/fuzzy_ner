from typing import List

import fuzzy_ner
import pytest


class TestFuzzyNerPackage:
    pytest.mark.parametrize("input, synonyms, match, score", [
        ("helo", ["Hello", "dog"], "hello", 0.8)
    ])
    def test_find_matches(
            self,
            input: str,
            synonyms: List[str],
            match: str,
            score: float,
    ) -> None:
        fn = fuzzy_ner.FuzzyNer(synonyms)
        matches = fn.find_matches(input)
        assert len(matches) == 1
        print(matches[0].synonym)
        print(matches[0].score)