# 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

echo "Installing wxWidgets"
sudo apt-get -y install libwxgtk3.0-gtk3-dev

if [ -d "imebra" ]; then
  echo 'Imebra is already installed, skip building Imebra'
else
  echo "Building imebra"
  mkdir imebra
  cd imebra
  wget https://github.com/binarno/imebra_mirror/archive/refs/heads/4.0.8.zip
  unzip *.zip
  cd imebra_mirror-4.0.8/
  cp -a . ..
  cd ..
  mkdir artifacts
  cd artifacts
  cmake ../library -DIMEBRA_SHARED_STATIC=STATIC
  cmake --build .
  cd ..
  cd ..
fi

echo "Starting installing"
echo "Installing core module"
cd gi_core
make install
cd ..
echo "Installing additions module"
cd gi_additions
make install
cd ..
echo "Installing wrapper module"
cd gi_wrapper_cpp
make install
cd ..
echo "Installing GUI module"
cd gi_gui_qt
make install
cd ..
echo "Finish installing"
