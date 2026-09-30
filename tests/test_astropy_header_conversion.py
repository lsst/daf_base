# This file is part of daf_base
#
# Developed for the LSST Data Management System.
# This product includes software developed by the LSST Project
# (http://www.lsst.org/).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import unittest

import astropy.io.fits

import lsst.utils.tests
from lsst.daf.base import PropertyList


def _card80(image: str) -> str:
    """Pad a FITS card image to exactly 80 characters."""
    return image.ljust(80)[:80]


class AstropyHeaderConversionTestCase(lsst.utils.tests.TestCase):
    """Tests for PropertyList.from_astropy_header and to_astropy_header."""

    def test_header_round_trip_through_property_list(self):
        """Test that an Astropy header converted to PropertyList and back
        keeps all of its cards, including duplicated keywords, comments, and
        HISTORY and COMMENT cards.
        """
        header = astropy.io.fits.Header()
        header.append(("PLATFORM", "lsstcam"), end=True)
        header.append(("BGMEAN", 1.5), end=True)
        header.append(("BGMEAN", 2.5), end=True)
        header.append(astropy.io.fits.Card("ZPT", 27.3, "derived zeropoint"), end=True)
        header.add_history("made by lsst_pipe")
        header.add_history("processed by cp_pipe")
        # Text containing "=" makes astropy treat this as a valueless card.
        header.add_comment("derived zeropoint = 27.3")
        header.append(astropy.io.fits.Card("", ""), end=True)  # blank card
        header["EXPTIME"] = 30.0

        pl = PropertyList.from_astropy_header(header)
        # Duplicated keys keep all of their values in the PropertyList.
        self.assertEqual(pl.getArray("BGMEAN"), [1.5, 2.5])
        self.assertEqual(
            pl.getArray("HISTORY"), ["made by lsst_pipe", "processed by cp_pipe"]
        )
        self.assertEqual(pl.getComment("ZPT"), "derived zeropoint")

        round_tripped = pl.to_astropy_header()
        # Blank cards are dropped; everything else keeps its keyword, value,
        # and comment, in order.
        expected = [
            (card.keyword, card.value, card.comment) for card in header.cards if card.keyword
        ]
        self.assertEqual(
            [(card.keyword, card.value, card.comment) for card in round_tripped.cards],
            expected,
        )
        # Test that HISTORY and COMMENT cards render without "= value" to make
        # sure we passed them back to Astropy correctly.
        self.assertEqual(
            [str(card).rstrip() for card in round_tripped.cards if card.keyword == "HISTORY"],
            ["HISTORY made by lsst_pipe", "HISTORY processed by cp_pipe"],
        )
        self.assertEqual(
            [str(card).rstrip() for card in round_tripped.cards if card.keyword == "COMMENT"],
            ["COMMENT derived zeropoint = 27.3"],
        )

    def test_property_list_metadata_round_trip_through_header(self):
        """Test that a PropertyList converted to an Astropy header and back
        keeps all of its values and comments, including duplicated keys and
        HISTORY and COMMENT entries.
        """
        metadata = PropertyList()
        metadata["PLATFORM"] = "lsstcam"
        metadata.add("BGMEAN", 1.5)
        metadata.add("BGMEAN", 2.5)
        metadata.add("SEEING", 1.2, "arcsec")
        metadata.add("HISTORY", "made by lsst_pipe")
        metadata.add("HISTORY", "processed by cp_pipe")
        metadata.add("COMMENT", "derived zeropoint = 27.3")
        metadata.add("BGMEAN", 3.5)

        header = metadata.to_astropy_header()
        self.assertEqual(
            [(card.keyword, card.value) for card in header.cards],
            [
                ("PLATFORM", "lsstcam"),
                ("BGMEAN", 1.5),
                ("BGMEAN", 2.5),
                ("BGMEAN", 3.5),
                ("SEEING", 1.2),
                ("HISTORY", "made by lsst_pipe"),
                ("HISTORY", "processed by cp_pipe"),
                ("COMMENT", "derived zeropoint = 27.3"),
            ],
        )
        self.assertEqual(
            [card.comment for card in header.cards if card.keyword == "SEEING"], ["arcsec"]
        )
        self.assertEqual(
            [str(card).rstrip() for card in header.cards if card.keyword == "HISTORY"],
            ["HISTORY made by lsst_pipe", "HISTORY processed by cp_pipe"],
        )

        round_tripped = PropertyList.from_astropy_header(header)
        self.assertEqual(round_tripped.getArray("BGMEAN"), [1.5, 2.5, 3.5])
        self.assertEqual(
            round_tripped.getArray("HISTORY"), ["made by lsst_pipe", "processed by cp_pipe"]
        )
        self.assertEqual(round_tripped.getArray("COMMENT"), ["derived zeropoint = 27.3"])
        self.assertEqual(round_tripped["PLATFORM"], "lsstcam")
        self.assertEqual(round_tripped.getScalar("SEEING"), 1.2)
        self.assertEqual(round_tripped.getComment("SEEING"), "arcsec")

    def test_header_to_property_list_undef_and_skips(self):
        """Test how cards with values are handled: unparsable values (real
        raws have headers with unterminated quoted strings) are skipped,
        while `Undefined` values (e.g. an empty ``SEEING =``) and value-less
        cards with comments are stored as undefined properties.
        """
        header = astropy.io.fits.Header.fromstring(
            "".join(
                [
                    _card80("PLATFORM= 'lsstcam'"),
                    _card80("SEEING  ="),
                    _card80("FOO     = / comment only"),
                    _card80("PROGRAM = 'HITS: real-time detection of &"),
                    _card80("CONTINUE  'stellar explosions&'"),
                    _card80("BGMEAN  = 1.5"),
                    _card80("BGMEAN  = 2.5"),
                ]
            )
        )
        # The unparsable and undefined cards must at least be present.
        self.assertIn("PROGRAM", header)
        self.assertIsInstance(
            header.cards["SEEING"].value, astropy.io.fits.card.Undefined
        )

        pl = PropertyList.from_astropy_header(header)
        self.assertEqual(pl["PLATFORM"], "lsstcam")
        self.assertEqual(pl.getArray("BGMEAN"), [1.5, 2.5])
        # Undefined cards are stored as undefined, not skipped.
        self.assertTrue(pl.exists("SEEING"))
        self.assertTrue(pl.isUndefined("SEEING"))
        self.assertTrue(pl.exists("FOO"))
        self.assertTrue(pl.isUndefined("FOO"))
        self.assertEqual(pl.getComment("FOO"), "comment only")
        # Unparsable values have no PropertyList representation at all.
        self.assertFalse(pl.exists("PROGRAM"))

        round_tripped = pl.to_astropy_header()
        self.assertEqual(
            [
                (card.keyword, isinstance(card.value, astropy.io.fits.card.Undefined))
                for card in round_tripped.cards
            ],
            [
                ("PLATFORM", False),
                ("SEEING", True),
                ("FOO", True),
                ("BGMEAN", False),
                ("BGMEAN", False),
            ],
        )
        self.assertEqual(
            [card.comment for card in round_tripped.cards if card.keyword == "FOO"],
            ["comment only"],
        )


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()
