document.addEventListener("DOMContentLoaded", () => {
	const fileList = document.getElementById("file-list");

	async function fetchFiles() {
		try {
			const response = await fetch("/files");
			const files = await response.json();

			fileList.innerHTML = "";

			files.forEach(file => {
				const listItem = document.createElement("li");
				listItem.textContent = file;

				const deleteButton = document.createElement("button");
				deleteButton.textContent = "Delete";
				deleteButton.onclick = () => deleteFile(file);

				listItem.appendChild(deleteButton);
				fileList.appendChild(listItem);
			});
		} catch (error) {
			console.error("Error fetching files:", error);
		}
	}

	async function deleteFile(fileName) {
		try {
			const response = await fetch(`/delete?file=${encodeURIComponent(fileName)}`, {
				method: "DELETE"
			});

			if (response.ok) {
				fetchFiles();
			} else {
				console.error("Error deleting file:", await response.text());
			}
		} catch (error) {
			console.error("Error deleting file:", error);
		}
	}

	fetchFiles();
});
