using System;
using System.Windows;
using System.Windows.Forms;
using System.Text;
using System.Data;
using System.Collections;
using System.Diagnostics;

namespace Bin
{
	/// -----------------------------------------------------------------------------
	/// Project	 : Bin
	/// Class	 : HexGrid
	/// 
	/// Use of this software is privilege of CodeProject members. Feel free to modify
	/// and report any bugs to Codeproject.com.
	/// 
	/// -----------------------------------------------------------------------------
	/// <summary>
	/// This class helps when editing several hexadecimal numbers.
	/// </summary>
	/// <remarks>
	/// </remarks>
	/// <history>
	/// 	[Rudy Guzman]	4/10/2005	Created
	/// </history>
	/// -----------------------------------------------------------------------------
	public class HexGrid : DataGrid
	{
		public DataTable dataTable = new DataTable();
		private int numBytes, numCols;
		
		public HexGrid()
		{
			numBytes = 0;
			numCols = 16;
			base.PreferredColumnWidth = 20;
			base.CaptionVisible = false;
			base.ColumnHeadersVisible = false;
			base.RowHeadersVisible = false;
		}

		//----------------------------------------------------------------------------------------------------------------------
		//			Properties
		//----------------------------------------------------------------------------------------------------------------------
		public int NumColumns
		{
			get { return numCols;}
			set 
			{
				if (value == 0)
					MessageBox.Show("Invalid parameter. Can't set number of columns in grid to 0");
				else
					numCols = value;
			}
		}

		public int NumBytes
		{
			get { return numBytes; }
		}

		//----------------------------------------------------------------------------------------------------------------------
		//			Public Methods
		//----------------------------------------------------------------------------------------------------------------------
		public void FillGrid(string data)
		{
			dataTable = null;
			if (data.Length > 0)
			{
				numBytes = data.Length / 2;
				byte [] byteArray = new Hex(data).ToBytes();
				dataTable = createDataTable(byteArray);
			}
			this.DataSource = dataTable;
		}

		public string GetValueFrom(int bytePosition, int bitPosition, int numBits)
		{
			byte [] bytesFromTable = getBytesFromTable();
			Bits allBits = new Bits(bytesFromTable);
			System.Diagnostics.Debug.WriteLine(allBits.ToString());
			Bits extractedBits = allBits.GetBits(bytePosition, bitPosition, numBits);
			return extractedBits.ToString();
//			return new Bits(bytesFromTable).GetBits(bitPosition, numBits).ToString();
		}

		//----------------------------------------------------------------------------------------------------------------------
		//			Private Methods
		//----------------------------------------------------------------------------------------------------------------------
		private byte [] getBytesFromTable()
		{
			int currByte = 0;
			StringBuilder sb = new StringBuilder();;
			string currCell;
			bool valueTruncated = false;
			for (int row = 0; row < dataTable.Rows.Count && currByte < numBytes; row++)
			{
				for (int col = 0; col < this.numCols && currByte < numBytes; col++)
				{
					currCell = dataTable.Rows[row][col].ToString();
					if (currCell.Length == 0)
						sb.Append("00");
					else if (currCell.Length == 1)
						sb.Append("0" + currCell);
					else if (currCell.Length == 2)
						sb.Append(currCell);
					else
					{
						sb.Append(currCell.Substring(0, 2));
						valueTruncated = true;
					}
					currByte++;
				}
			}
			if (valueTruncated)
				MessageBox.Show("Some cells had more than 2 characters so they were truncated");

			return new Bin.Hex(sb.ToString()).ToBytes();
		}

		private DataTable createDataTable(byte [] byteArray)
		{
			DataTable table = new DataTable();
			//calculate number of rows
			int numRows = byteArray.Length / numCols + 1;

			//create the columns
			for (int col = 0; col < numCols; col++)
			{
				table.Columns.Add(col.ToString());
//				table.Columns[col].MaxLength = 2;
			}

			//store bytes in the table
			string [] rows = new string[numCols];
			numBytes = byteArray.Length;
			int currByte = 0;
			for (int row = 0; row < numRows; row++)
			{
				for (int i = 0; i < numCols; i++) rows[i] = "";
				for (int col = 0; col < numCols && currByte < byteArray.Length; col++)
				{
					rows[col] = byteArray[currByte].ToString("X2");
					currByte++;
				}
				table.Rows.Add(rows);
			}

			return table;
		}
	}
}
